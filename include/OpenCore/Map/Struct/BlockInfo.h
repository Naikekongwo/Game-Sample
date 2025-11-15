#ifndef _BLOCKINFO_H_
#define _BLOCKINFO_H_

// BlockInfo.h
// 方格信息类

#include <cstdint>

// 此处定义地图的格式
// MapLayers 指地图的层级
// lengthInfo 分别指代各层级的长度（单位：字节）
constexpr uint8_t MapLayers = 4;
constexpr uint8_t lengthInfo[] = {1,1,1,1};

struct BlockInfo
{
    uint16_t Info[MapLayers];
};

#endif //_BLOCKINFO_H_