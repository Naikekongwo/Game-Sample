#include "OpenCore/OpenCore.hpp"
#include <stdexcept>

// 来实现地图的管理器吧

bool MapManager::registerMap(unique_ptr<OpenCoreMap> map)
{
    if (MapPool_.contains(map->id))
    {
        throw std::runtime_error("Conflict between maps.\n");
        return false;
    }

    // map->onEnter();

    MapPool_[map->id] = std::move(map);

    return true;
}
bool MapManager::downloadMap(OpenCoreMap *map) { return true; }