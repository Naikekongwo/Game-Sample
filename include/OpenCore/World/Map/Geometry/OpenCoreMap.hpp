#pragma once

// OpenCoreMap
// 基本的地图基类

#include "MapStruct.hpp"

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#define SCISSOR_X 32
#define SCISSOR_Y 18

#define CHUNK_WIDTH 5

// 地图暂且设定为单文件最多储存16x16个区块的地图
// 则代表储存区块的长或高需要4位二进制，因故只需要读取一字节作为头数据
using std::shared_ptr;
using std::string;
using std::vector;

enum class MapStatus
{
    Registered,
    Loaded,
    Frozen
};

class OpenCoreMap
{
  public:
    // 基类注册地图
    explicit OpenCoreMap(short id, const string &mapPath)
        : id(id), mapPath(mapPath)
    {
        if (id == 0 or mapPath.empty())
        {
            throw std::runtime_error(
                "Failed to load map, 'cause something was missing or wrong.\n");
        }
    }

    virtual ~OpenCoreMap() {};

    // 地图加载的入口函数
    // 可以是初始化阶段读取, 也可以是内存冻结之后恢复
    virtual bool onEnter() = 0;

    // 获取地图的宽高
    virtual uint16_t getMapWidth() const noexcept { return MapWidth; }
    virtual uint16_t getMapHeight() const noexcept { return MapHeight; }

    // 获取指定位置的方块信息
    virtual BlockInfo &getBlockInfo(int offsetX, int offsetY) = 0;

    virtual void onUpdate(float totalTime) {};
    virtual void onExit() = 0;

    // 地图是否已经完整加载到内存中
    virtual bool ready() const noexcept { return status == MapStatus::Loaded; }

    short id;
    string mapPath;
    MapStatus status = MapStatus::Registered;

  protected:
    uint16_t MapWidth = 0;
    uint16_t MapHeight = 0;
};