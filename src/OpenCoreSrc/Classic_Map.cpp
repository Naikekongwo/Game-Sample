#include "OpenCore/OpenCore.hpp"

// 地图的入口/又称加载的函数,其返回值代表是否初始化成功
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

    // 无需判断该坐标是否存在，因为已经做了判断(返回Rect判断坐标范围的合法程度)
    // 一定要判断！

    if ((offsetX < 0 or offsetX >= MapWidth) or
        (offsetY < 0 or offsetY >= MapHeight))
    {
        return emptyBlock;
    }

    // 如果坐标合法，那么开始传输

    return Data[offsetX + offsetY * MapWidth];

    // 传输回指定的数据
}

void ClassicMap::onExit() {}