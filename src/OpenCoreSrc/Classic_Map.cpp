#include "OpenCore/OpenCore.hpp"

void ClassicMap::onEnter()
{
    this->WindowHeight = OpenCoreManagers::SetManager.getRenderHeight();
    this->WindowWidth = OpenCoreManagers::SetManager.getRenderWidth();
    // 确认了渲染窗口的大小

    LoadMapFromFile(mapPath, Data, MapWidth, MapHeight);
}

BlockInfo &ClassicMap::getBlockInfo(int offsetX, int offsetY)
{
    // 获取某坐标的数据
    // 这里的offsetX和offsetY代表的是玩家所处的逻辑坐标
    // 对于ClassicMap来说，逻辑坐标就代表地图的坐标

    // 无需判断该坐标是否存在，因为已经做了判断(返回Rect判断坐标范围的合法程度)
    // 一定要判断！

    if (offsetX < 0 or offsetY < 0)
    {
        return Data[0];
    }

    // 如果坐标合法，那么开始传输

    return Data[offsetX + offsetY * MapWidth];

    // 传输回指定的数据
}

void ClassicMap::onExit() {}

void ClassicMap::onUpdate(float totalTime) {}