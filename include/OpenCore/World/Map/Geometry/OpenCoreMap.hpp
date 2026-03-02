#pragma once

// OpenCoreMap
// 基本的地图基类

#include <stdexcept>
#include <string>
#include <vector>

#define SCISSOR_X 32
#define SCISSOR_Y 18

#define CHUNK_WIDTH 5
#define MAP_INFO_LENGTH_BYTE 1

// 地图暂且设定为单文件最多储存16x16个区块的地图
// 则代表储存区块的长或高需要4位二进制，因故只需要读取一字节作为头数据
using std::string;
using std::vector;

struct BlockInfo
{
    unsigned char Terrain = 1;

    BlockInfo(vector<uint8_t> ReadIn)
    {
        // 开始顺序读取
        // Terrain 地形
        Terrain = ReadIn[0];
    }
};

class OpenCoreMap
{
  public:
    // 基类注册地图
    OpenCoreMap(short id, const string &mapPath)
    {
        if (id == 0 or mapPath.empty())
        {
            throw std::runtime_error(
                "Failed to load map, 'cause something was missing or wrong.\n");
        }
        this->id = id;
        this->mapPath = mapPath;
    }

    virtual ~OpenCoreMap() {};

    virtual void Init() = 0;

    virtual void onUpdate(float totalTime) = 0;
    virtual void onRender() = 0;

    short id;
    string mapPath;
};