#include "OpenCore/OpenCore.hpp"
#include <memory>
#include <stdexcept>

// 来实现地图的管理器吧

bool MapManager::registerClassicMap(short id, string path)
{
    auto map = std::make_unique<ClassicMap>(id, path);

    return registerMap(std::move(map));
}

bool MapManager::registerMap(unique_ptr<OpenCoreMap> map)
{
    if (MapPool_.contains(map->id))
    {
        throw std::runtime_error("Conflict between maps.\n");
        return false;
    }
    MapPool_[map->id] = std::move(map);
    return true;
}
bool MapManager::downloadMap(OpenCoreMap *map) { return true; }