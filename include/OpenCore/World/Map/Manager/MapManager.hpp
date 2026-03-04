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
    bool registerClassicMap(short id, string path);
    bool registerMap(unique_ptr<OpenCoreMap> map);
    bool downloadMap(OpenCoreMap *map);

    // 获取地图相关信息的函数
    uint16_t getMapWidth() const noexcept
    {
        return MapPool_.at(currentID)->getMapWidth();
    }
    uint16_t getMapHeight() const noexcept
    {
        return MapPool_.at(currentID)->getMapHeight();
    }

    BlockInfo &getBlockInfo(int offsetX, int offsetY)
    {
        return MapPool_.at(currentID)->getBlockInfo(offsetX, offsetY);
    }

    short getCurrentID() const noexcept { return currentID; }
    void setCurrentID(short id) { currentID = id; }

    // 判断地图管理器是否为空
    // 判断是否包含某地图
    // 判断某地图是否准备完毕
    bool empty() const noexcept { return MapPool_.empty(); }
    bool contains(short mapID) const noexcept
    {
        return MapPool_.contains(mapID);
    }
    bool legal() const noexcept
    {
        if (!contains(currentID) or empty())
            return false;

        return MapPool_.at(currentID)->ready();
    }

    void ActivateMap()
    {
        if (MapPool_.contains(currentID))
        {
            if (!MapPool_.at(currentID)->ready())
            {
                MapPool_.at(currentID)->Activate();
            }
        }
    }

  private:
    short currentID = 0;
    std::unordered_map<short, unique_ptr<OpenCoreMap>> MapPool_;
};