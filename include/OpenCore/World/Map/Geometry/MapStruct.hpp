#pragma once

// MapStruct
// 地图的基本数据结构

#include <cstdint>
#include <vector>

using std::vector;

struct MapHeader
{
    // magic[4] : "OCMP" 此为OpenCoreMap结构的标识符
    // version : 此为地图版本
    // width, height : 此为地图大小
    // layerCount : 此为地图层级
    // reserved : 此为保留数目
    char magic[4];
    uint8_t version;
    uint16_t width;
    uint16_t height;
    uint8_t layerCount;
    uint8_t reserved[25];
};

struct BlockInfo
{
    // Terrain : 地形
    uint8_t Terrain = 1;

    BlockInfo(vector<uint8_t> datas)
    {
        if (datas.size() > sizeof(BlockInfo))
        {
            throw std::runtime_error(
                "BlockInfo too big! Are you using new version map?");
        }

        Terrain = (datas.size() >= 1) ? datas[0] : 0;
    }
};
