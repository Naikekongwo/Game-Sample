#ifndef _MAP_H_
#define _MAP_H_

// Map.h
// 地图的基类

#include <cstdint>
#include <string>

class BlockInfo;
class MapManager;

class Map
{
  public:
    // 默认的初始化函数
    explicit Map(const std::string MapPath) : MapPath(MapPath) {};

    // 获取指定坐标的信息
    BlockInfo getBlockInfo(int16_t x, int16_t y);
    // 修改指定坐标的信息
    bool setBlockInfo(int16_t x, int16_t y);

  protected:
    // 地图的具体地址
    std::string MapPath;
    // 地图管理器的具体地址
    MapManager *MapMgr;
};

#endif //_MAP_H_