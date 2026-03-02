#include "OpenCore/OpenCore.hpp"

void ClassicMap::onEnter()
{
    this->WindowHeight = OpenCoreManagers::SetManager.getRenderHeight();
    this->WindowWidth = OpenCoreManagers::SetManager.getRenderWidth();

    // 确认了渲染窗口的大小

    // 读取地图
    std::ifstream file(mapPath, std::ios::binary);
    if (!file)
        throw std::runtime_error("Failed to open map file: " + mapPath);

    // 1. 读取头部 32 字节
    MapHeader header;
    file.read(reinterpret_cast<char *>(&header), sizeof(header));
    if (!file)
        throw std::runtime_error("Failed to read map header");

    // 检查魔数
    if (header.magic[0] != 'O' || header.magic[1] != 'C' ||
        header.magic[2] != 'M' || header.magic[3] != 'P')
    {
        throw std::runtime_error("Invalid map file format");
    }

    // 解析宽高
    MapWidth = header.width;
    MapHeight = header.height;

    // 2. 读取 BlockInfo 数据
    size_t totalBlocks = static_cast<size_t>(MapWidth) * MapHeight;
    Data.reserve(totalBlocks);

    std::vector<uint8_t> buffer(BlockInfo::getSize());
    for (size_t i = 0; i < totalBlocks; ++i)
    {
        file.read(reinterpret_cast<char *>(buffer.data()), buffer.size());
        if (!file)
            throw std::runtime_error("Unexpected EOF while reading block data");

        Data.emplace_back(buffer);
    }

    file.close();
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