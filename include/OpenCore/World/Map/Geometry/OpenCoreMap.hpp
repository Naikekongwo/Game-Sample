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

    virtual void onEnter() = 0;

    virtual uint16_t getMapWidth() const noexcept { return MapWidth; }
    virtual uint16_t getMapHeight() const noexcept { return MapHeight; }

    virtual void onUpdate(float totalTime) = 0;

    virtual void onExit() = 0;

    virtual BlockInfo &getBlockInfo(int offsetX, int offsetY) = 0;

    short id;
    string mapPath;

    shared_ptr<uint8_t> WindowWidth = std::make_shared<uint8_t>(16);
    shared_ptr<uint8_t> WindowHeight = std::make_shared<uint8_t>(9);

    vector<BlockInfo> Data;

    uint16_t MapWidth = 0;
    uint16_t MapHeight = 0;
};