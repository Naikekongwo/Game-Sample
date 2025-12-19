#include "OpenCore/OpenCore.hpp"
#include "Eclipsea/Eclipsea.hpp"


std::future<void> ResourceManager::LoadManifest(short id) {
    // 1. 构建文件名
    std::string filename = "assets/script/STAGE_" + std::to_string(id) + ".json";
    
    // 2. 检查文件是否存在
    FILE* file = fopen(filename.c_str(), "rb");
    if (!file) {
        std::printf("Error: JSON file %s does not exist", filename.c_str());
        // 返回一个已经完成的 future（防止外部阻塞）
        std::promise<void> p;
        p.set_value();
        return p.get_future();
    }

    // 3. 解析JSON文件
    char readBuffer[65536];
    rapidjson::FileReadStream stream(file, readBuffer, sizeof(readBuffer));
    rapidjson::Document doc;
    doc.ParseStream(stream);
    fclose(file);

    // 4. 检查解析错误
    if (doc.HasParseError()) {
        std::printf("JSON parse error (%s): %s", filename.c_str(), rapidjson::GetParseError_En(doc.GetParseError()));
        std::promise<void> p;
        p.set_value();
        return p.get_future();
    }

    // 5. 验证JSON根对象
    if (!doc.IsObject()) {
        std::printf("Error: JSON root is not an object in %s", filename.c_str());
        std::promise<void> p;
        p.set_value();
        return p.get_future();
    }

    // 6. 存储异步任务future
    std::vector<std::future<void>> futures;

    // 7. 遍历所有资源对象
    for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
        short resourceId;
        try {
            resourceId = static_cast<short>(std::stoi(it->name.GetString()));
        } catch (...) {
            std::printf("Invalid resource ID format: %s", it->name.GetString());
            continue;
        }

        const rapidjson::Value& resObj = it->value;
        if (!resObj.IsObject() || 
            !resObj.HasMember("path") || !resObj["path"].IsString() ||
            !resObj.HasMember("category") || !resObj["category"].IsString()) {
            std::printf("Invalid resource object for ID: %d", resourceId);
            continue;
        }

        std::string path = resObj["path"].GetString();
        std::string category = resObj["category"].GetString();

        if (category == "music") {
            futures.push_back(LoadMusicAsync(resourceId, path));
        } else if (category == "texture") {
            futures.push_back(LoadTextureAsync(resourceId, path));
            std::printf("📦 LoadResourcesFromJson: Added texture id=%d to wait list\n", resourceId);
        } else if (category == "font") {
            if(!resObj.HasMember("size") || !resObj["size"].IsInt()) {
                std::printf("Font resource missing or invalid font size for resource ID: %d", resourceId);
                continue;
            }
            int size = resObj["size"].GetInt();
            futures.push_back(LoadFontAsync(resourceId, path, size));
        } else {
            std::printf("Unknown category '%s' for resource ID: %d", category.c_str(), resourceId);
        }
    }

    // 8. 返回一个 future 表示所有任务完成
    return std::async(std::launch::async, [futures = std::move(futures), filename]() mutable {
        std::printf("⏳ LoadResourcesFromJson: Waiting for all resources...\n");
        
        for (auto& future : futures) {
            future.wait();  // 🔥 现在这个wait包括等待GPU纹理转换
        }
        
        std::printf("✅ LoadResourcesFromJson: ALL resources (including GPU textures) loaded from %s\n", 
                   filename.c_str());
    });
}