#pragma once

// MapManager.hpp
// 管理地图类的管理类

#include <memory>
#include <unordered_map>

#include "OpenCore/World/Map/Geometry/Classic_Map.hpp"
#include "OpenCore/World/Map/Geometry/Hybrid_Map.hpp"
#include "OpenCore/World/Map/Geometry/OpenCoreMap.hpp"

// #include "OpenCore/Map/Concepts/SnakeMap.hpp"

using std::unique_ptr;

class MapManager final
{
  public:
    // 单例模式
    static MapManager &getInstance()
    {
        static MapManager instance;
        return instance;
    }

    bool registerMap(unique_ptr<OpenCoreMap> map);
    bool downloadMap(OpenCoreMap *map);

  private:
    // 地图池
    std::unordered_map<short, unique_ptr<OpenCoreMap>> MapPool_;
};