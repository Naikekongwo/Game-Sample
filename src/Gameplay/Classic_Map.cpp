#include "Eclipsea/Gameplay/Map/Geometry/Classic_Map.hpp"
#include "Eclipsea/Gameplay/Map/MapLoader.hpp"
#include "OpenCore.hpp"

// 地图的入口，又称加载的函数，其返回值代表是否初始化成功
bool ClassicMap::onEnter()
{
    LoadMapFromFile(mapPath, Data, MapWidth, MapHeight);
    status = MapStatus::Loaded;

    LOG("地图加载成功 路径: {}，宽:{}，高:{}", mapPath.c_str(), MapWidth,
        MapHeight);
    return true;
}

BlockInfo &ClassicMap::getBlockInfo(int offsetX, int offsetY)
{
    // 获取某坐标的数据
    // 这里的offsetX和offsetY代表的是玩家所处的逻辑坐标
    // 对于ClassicMap来说，逻辑坐标就代表地图的坐标

    // 需要判断坐标是否合法
    if ((offsetX < 0 || offsetX >= MapWidth) ||
        (offsetY < 0 || offsetY >= MapHeight))
    {
        return emptyBlock;
    }

    return Data[offsetX + offsetY * MapWidth];
}

void ClassicMap::onExit() {}
