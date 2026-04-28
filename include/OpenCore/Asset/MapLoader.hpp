#include "OpenCore/Core/Helpers/Debugger.hpp"
#include "OpenCore/World/Map/Geometry/MapStruct.hpp"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

using std::string;

struct MapInfo
{
    std::string path;   // 地图文件路径
    uint16_t width;     // 地图宽度（块数）
    uint16_t height;    // 地图高度（块数）
    uint8_t version;    // 地图格式版本
    uint8_t layerCount; // 地图层级数
    uint8_t blockSize;  // 地图单元大小
};

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

    if (header.blockSize == 0)
        header.blockSize = 1;

    // LOG("版本:{}, 层级:{}, 宽:{}, 高:{}, 魔数:{}", header.version,
    //     header.layerCount, header.width, header.height, header.magic);

    size_t totalBlocks = static_cast<size_t>(mapWidth) * mapHeight;
    datas.reserve(totalBlocks);

    std::vector<uint8_t> buffer(sizeof(header.blockSize));
    for (size_t i = 0; i < totalBlocks; ++i)
    {
        file.read(reinterpret_cast<char *>(buffer.data()), buffer.size());
        if (!file)
            throw std::runtime_error("Unexpected EOF while reading block data");

        datas.emplace_back(buffer);
    }

    file.close();
}

inline MapInfo LoadMapInfo(const std::string &mapPath)
{
    std::ifstream file(mapPath, std::ios::binary);
    if (!file)
        throw std::runtime_error("Failed to open map file: " + mapPath);

    MapHeader header;
    file.read(reinterpret_cast<char *>(&header), sizeof(header));
    if (!file)
        throw std::runtime_error("Failed to read map header");

    // 验证魔数
    if (header.magic[0] != 'O' || header.magic[1] != 'C' ||
        header.magic[2] != 'M' || header.magic[3] != 'P')
    {
        throw std::runtime_error("Invalid map file format");
    }

    // 填充 MapInfo
    MapInfo info;
    info.path = mapPath;
    info.width = header.width;
    info.height = header.height;
    info.version = header.version;
    info.layerCount = header.layerCount;
    info.blockSize = header.blockSize;

    return info;
}

/// @brief 遍历文件夹，解析所有 .ocmp 地图文件，返回元信息数组。
/// @param folderPath 文件夹路径（允许相对路径）
/// @return 包含所有成功解析的 MapInfo 的 vector
/// @note 如果文件无法打开或格式无效，会跳过该文件并在标准错误流输出警告。
inline std::vector<MapInfo>
LoadMapInfosFromFolder(const std::string &folderPath)
{
    namespace fs = std::filesystem;
    std::vector<MapInfo> result;

    if (!fs::exists(folderPath) || !fs::is_directory(folderPath))
    {
        throw std::runtime_error("Invalid folder path: " + folderPath);
    }

    for (const auto &entry : fs::directory_iterator(folderPath))
    {
        // 检查扩展名是否为 .ocmp
        if (entry.path().extension() == ".ocmp" && fs::is_regular_file(entry))
        {
            try
            {
                MapInfo info = LoadMapInfo(entry.path().string());
                result.push_back(std::move(info));
            }
            catch (const std::exception &e)
            {
                LOG("Warning: 加载失败 ,路径: {}, 错误:{}",
                    entry.path().string(), e.what());
            }
        }
    }

    return result;
}