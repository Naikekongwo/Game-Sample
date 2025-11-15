#ifndef _MAPMANAGER_H_
#define _MAPMANAGER_H_

// MapManager.h
// 地图管理器

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>


#include "OpenCore/Map/Struct/Map.h"

class MapManager
{
  public:
    // 单例类的初始化
    static MapManager& getInstance()
    {
        static MapManager instance;
        return instance;
    }

  private:
    // 当前的地图
    std::unique_ptr<Map> currentMap;
    // 休眠的地图
    std::unordered_map<std::string, std::unique_ptr<Map>> legacyMaps;
    // 共享信息池：即多个地图中共享的信息
    std::unordered_map<uint16_t, uint8_t> libraries;
};

#endif // _MAPMANAGER_H_