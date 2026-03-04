#include "OpenCore/OpenCore.hpp"

void ClassicMap::onEnter()
{
    LoadMapFromFile(mapPath, Data, MapWidth, MapHeight);
    status = MapStatus::Loaded;
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

bool ClassicMap::Activate()
{
    switch (status)
    {
    case MapStatus::Registered:
    {
        // 仅仅注册了，还未加载
        onEnter();
        break;
    }
    case MapStatus::Loaded:
    {
        break;
    }
    case MapStatus::Frozen:
    {
        break;
    }
    default:
    }
    return true;
}