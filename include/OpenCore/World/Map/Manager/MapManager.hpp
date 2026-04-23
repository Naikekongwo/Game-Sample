#pragma once

// MapManager.hpp
// 管理地图类的管理类

#include <memory>
#include <unordered_map>

#include "OpenCore/World/Map/Geometry/Classic_Map.hpp"
#include "OpenCore/World/Map/Geometry/Hybrid_Map.hpp"
#include "OpenCore/World/Map/Geometry/OpenCoreMap.hpp"

using std::unique_ptr;

class MapManager final
{
  public:
    bool loadClassicMap(short id, string path);
    bool addMap(unique_ptr<OpenCoreMap> map);
    bool saveMap(OpenCoreMap *map);

    // 获取地图相关信息的函数
    uint16_t getMapWidth() const noexcept
    {
        return MapPool_.at(currentID)->getMapWidth();
    }
    uint16_t getMapHeight() const noexcept
    {
        return MapPool_.at(currentID)->getMapHeight();
    }

    // 方块信息的转发函数
    BlockInfo &getBlockInfo(int offsetX, int offsetY)
    {
        return MapPool_.at(currentID)->getBlockInfo(offsetX, offsetY);
    }

    short getCurrentID() const noexcept { return currentID; }
    void setCurrentID(short id) { currentID = id; }

    bool contains(short mapID) const noexcept
    {
        return MapPool_.contains(mapID);
    }

    bool isReady() const noexcept
    {
        if (!contains(currentID) or MapPool_.empty())
            return false;

        return MapPool_.at(currentID)->ready();
    }

    bool initCurrentMap() const noexcept
    {
        if (isReady())
            return false;

        return MapPool_.at(currentID)->onEnter();
    }

  private:
    short currentID = 0;
    std::unordered_map<short, unique_ptr<OpenCoreMap>> MapPool_;
};