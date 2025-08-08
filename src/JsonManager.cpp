#include "OpenCore/OpenCore.hpp"
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/prettywriter.h>

// 单例初始化
JsonManager& JsonManager::getInstance() {
    static JsonManager instance;
    return instance;
}

// 构造函数
JsonManager::JsonManager() {
    initFilesystem();
    loadJson();
}

// 初始化文件系统
void JsonManager::initFilesystem() {
    std::lock_guard<std::mutex> lock(mutex);
    if (!fs::exists(jsonDir)) {
        fs::create_directory(jsonDir);
    }
}

// 加载JSON数据
void JsonManager::loadJson() {
    std::lock_guard<std::mutex> lock(mutex);
    document.SetObject();

    if (!fs::exists(mainJsonPath)) {
        saveJson();
        return;
    }

    std::ifstream ifs(mainJsonPath);
    if (!ifs.is_open()) {
        std::cerr << "Failed to open JSON file" << std::endl;
        return;
    }

    IStreamWrapper isw(ifs);
    document.ParseStream(isw);
    if (document.HasParseError()) {
        std::cerr << "JSON parse error: " << document.GetParseError() << std::endl;
        document.SetObject();
        return;
    }

    // 导入数据到Map
    for (auto& m : document.GetObject()) {
        const char* idStr = m.name.GetString();
        const char* value = m.value.GetString();
        try {
            short id = static_cast<short>(std::stoi(idStr));
            idToNameMap[id] = value;
            nameToIdMap[value] = id;
        } catch (...) {
            std::cerr << "Invalid ID format: " << idStr << std::endl;
        }
    }
}

// 保存JSON数据
void JsonManager::saveJson() {
    std::lock_guard<std::mutex> lock(mutex);
    std::ofstream ofs(mainJsonPath);
    if (!ofs.is_open()) {
        std::cerr << "Failed to create JSON file" << std::endl;
        return;
    }

    OStreamWrapper osw(ofs);
    PrettyWriter<OStreamWrapper> writer(osw);
    document.Accept(writer);
}

// 创建单个JSON文件
void JsonManager::createSingleJson(short id) {
    std::string filePath = jsonDir + "/" + std::to_string(id) + ".json";
    std::ofstream file(filePath);
    if (file.is_open()) file << "{}";
    else std::cerr << "Failed to create file: " << filePath << std::endl;
}

// 公开方法实现
// 通过场景id添加json
bool JsonManager::AddEntry(short id, const std::string& str) {
    auto& instance = getInstance();
    std::lock_guard<std::mutex> lock(instance.mutex);

    if (instance.idToNameMap.count(id) || instance.nameToIdMap.count(str)) {
        std::cerr << "ID or String already exists!" << std::endl;
        return false;
    }

    // 更新内存和JSON
    instance.idToNameMap[id] = str;
    instance.nameToIdMap[str] = id;
    Value idVal(std::to_string(id).c_str(), instance.document.GetAllocator());
    Value strVal(str.c_str(), instance.document.GetAllocator());
    instance.document.AddMember(idVal, strVal, instance.document.GetAllocator());
    instance.saveJson();
    instance.createSingleJson(id);
    return true;
}
//根据id获取名字
std::string JsonManager::GetNameById(short id) {
    auto& instance = getInstance();
    std::lock_guard<std::mutex> lock(instance.mutex);
    auto it = instance.idToNameMap.find(id);
    return (it != instance.idToNameMap.end()) ? it->second : "";
}
//根据名字获取id
short JsonManager::GetIdByName(const std::string& str) {
    auto& instance = getInstance();
    std::lock_guard<std::mutex> lock(instance.mutex);
    auto it = instance.nameToIdMap.find(str);
    return (it != instance.nameToIdMap.end()) ? it->second : -1;
}
//删除指定id的json文件
bool JsonManager::RemoveEntry(short id) {
    auto& instance = getInstance();
    std::lock_guard<std::mutex> lock(instance.mutex);
    
    auto it = instance.idToNameMap.find(id);
    if (it == instance.idToNameMap.end()) {
        std::cerr << "ID not found!" << std::endl;
        return false;
    }

    // 删除内存映射
    std::string str = it->second;
    instance.idToNameMap.erase(it);
    instance.nameToIdMap.erase(str);

    // 删除主JSON条目
    std::string idStr = std::to_string(id);
    if (instance.document.HasMember(idStr.c_str())) {
        instance.document.RemoveMember(idStr.c_str());
        instance.saveJson();
    }

    // 删除对应JSON文件
    std::string filePath = instance.jsonDir + "/" + idStr + ".json";
    if (fs::exists(filePath) && !fs::remove(filePath)) {
        std::cerr << "Failed to delete file: " << filePath << std::endl;
    }
    return true;
}
//向指定json中添加资源条目
//形如{
//    "1001": {
//        "path": "background.png",
//        "category": "texture"
//    }
//}

bool JsonManager::AddResourceEntry(short fileId, 
                             const std::string& path, 
                             short resourceId, 
                             const std::string& category) {
    // 创建类内静态mutex用于线程安全
    static std::mutex resMutex;
    std::lock_guard<std::mutex> lock(resMutex);
    
    // 构建完整文件路径（使用ConfigManager的json目录）
    auto& instance = getInstance();
    std::string filename = instance.jsonDir + "/" + std::to_string(fileId) + ".json";

    // 检查文件是否存在
    if (!fs::exists(filename)) {
        std::cerr << "File not found: " << filename << std::endl;
        return false;
    }
    
    // 验证类别
    if (category != "texture" && category != "music") {
        std::cerr << "Invalid category: " << category 
                  << " (must be 'texture' or 'music')" << std::endl;
        return false;
    }

    // 读取并解析JSON文件
    Document doc;
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    IStreamWrapper isw(ifs);
    doc.ParseStream(isw);
    if (doc.HasParseError()) {
        std::cerr << "JSON parse error in " << filename 
                  << ": " << doc.GetParseError() << std::endl;
        return false;
    }

    // 检查resourceId是否已存在
    std::string resIdStr = std::to_string(resourceId);
    if (doc.HasMember(resIdStr.c_str())) {
        std::cerr << "Resource ID " << resourceId << " already exists in " 
                  << filename << std::endl;
        return false;
    }

    // 检查name是否已存在
    for (auto& m : doc.GetObject()) {
        if (m.value.HasMember("path") && 
            m.value["path"].IsString() &&
            std::string(m.value["path"].GetString()) == path) {
            std::cerr << "Resource path '" << path << "' already exists in " 
                      << filename << std::endl;
            return false;
        }
    }

    // 添加新条目
    Value idVal(resIdStr.c_str(), doc.GetAllocator());
    Value resObj(kObjectType);
    Value nameVal(path.c_str(), doc.GetAllocator());
    Value catVal(category.c_str(), doc.GetAllocator());

    resObj.AddMember("path", nameVal, doc.GetAllocator());
    resObj.AddMember("category", catVal, doc.GetAllocator());
    doc.AddMember(idVal, resObj, doc.GetAllocator());

    // 保存修改
    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
        std::cerr << "Failed to save file: " << filename << std::endl;
        return false;
    }

    OStreamWrapper osw(ofs);
    PrettyWriter<OStreamWrapper> writer(osw);
    doc.Accept(writer);

    std::cout << "Successfully added resource to " << filename 
              << ": ID=" << resourceId << ", path=" << path 
              << ", category=" << category << std::endl;
    return true;
}

