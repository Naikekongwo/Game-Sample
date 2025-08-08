#ifndef _JSONMANAGER_H_
#define _JSONMANAGER_H_
#include <unordered_map>
#include <string>
#include <mutex>
#include <filesystem>
#include <rapidjson/document.h>

/* 组织方式大致是这样的：首先有一个main.json文件内部是场景id和场景名的对应关系，后续是场景id+.json的格式的一系列json文件，储存加载信息。 */


namespace fs = std::filesystem;
using namespace rapidjson;

class JsonManager {
private:

    // 单例实例（Meyers Singleton）
    static JsonManager& getInstance();

    // 成员变量
    std::unordered_map<short, std::string> idToNameMap;
    std::unordered_map<std::string, short> nameToIdMap;
    std::mutex mutex;
    const std::string jsonDir = "../json";
    const std::string mainJsonPath = jsonDir + "/main.json";
    Document document;

    // 私有构造/析构
    JsonManager();
    ~JsonManager() = default;

    // 内部方法（实现在.cpp中）
    void initFilesystem();
    void loadJson();
    void saveJson();
    void createSingleJson(short id);

public:
    // 禁用拷贝和赋值
    JsonManager(const JsonManager&) = delete;
    void operator=(const JsonManager&) = delete;


    // 公开接口
    static bool AddEntry(short id, const std::string& str);
    static std::string GetNameById(short id);
    static short GetIdByName(const std::string& str);
    static bool RemoveEntry(short id);
    static bool AddResourceEntry(short fileId, const std::string& name, short resourceId, const std::string& category);
};
#endif // _JSONMANAGER_H_
