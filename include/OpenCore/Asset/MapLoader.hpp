#include "OpenCore/World/Map/Geometry/MapStruct.hpp"

#include <cstdint>
#include <fstream>

using std::string;

/**
 * @brief 从二进制文件加载地图数据。
 * 
 * 读取由 MapHeader 和 BlockInfo 数组组成的地图文件。文件格式要求：
 * - 前 sizeof(MapHeader) 字节为头部信息，包含魔数 "OCMP"、地图宽度和高度。
 * - 随后是 mapWidth * mapHeight 个连续的 BlockInfo 对象（二进制格式）。
 * 
 * @param mapPath 地图文件的路径。
 * @param datas   [out] 存储读取到的 BlockInfo 对象，按行优先顺序排列。
 * @param mapWidth [out] 地图宽度（块数）。
 * @param mapHeight [out] 地图高度（块数）。
 * 
 * @throws std::runtime_error 当文件无法打开、读取失败或魔数不匹配时抛出异常。
 * 
 * @note BlockInfo 的二进制布局必须与写入时一致，否则数据解析会出错。
 */
inline void LoadMapFromFile(const string &mapPath, vector<BlockInfo> &datas,
                            uint16_t &mapWidth, uint16_t &mapHeight)
{
    std::ifstream file(mapPath, std::ios::binary);
    if (!file)
        throw std::runtime_error("Failed to open map file: " + mapPath);

    
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

    
    mapWidth = header.width;
    mapHeight = header.height;

    
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