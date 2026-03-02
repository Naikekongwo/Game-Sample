#include "OpenCore/World/Map/Geometry/MapStruct.hpp"

#include <cstdint>
#include <fstream>

using std::string;

inline void LoadMapFromFile(const string &mapPath, vector<BlockInfo> &datas,
                            uint16_t &mapWidth, uint16_t &mapHeight)
{
    std::ifstream file(mapPath, std::ios::binary);
    if (!file)
        throw std::runtime_error("Failed to open map file: " + mapPath);

    // 读取地图的头
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
    mapWidth = header.width;
    mapHeight = header.height;

    // 2. 读取 BlockInfo 数据
    size_t totalBlocks = static_cast<size_t>(mapWidth) * mapHeight;
    datas.reserve(totalBlocks);

    std::vector<uint8_t> buffer(sizeof(BlockInfo));
    for (size_t i = 0; i < totalBlocks; ++i)
    {
        file.read(reinterpret_cast<char *>(buffer.data()), buffer.size());
        if (!file)
            throw std::runtime_error("Unexpected EOF while reading block data");

        datas.emplace_back(buffer);
    }

    file.close();
}