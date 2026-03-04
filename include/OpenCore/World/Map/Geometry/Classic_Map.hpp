#pragma once

// Classic_Map.hpp
// 一次性读入的地图

#include "OpenCoreMap.hpp"
#include <corecrt.h>

static BlockInfo emptyBlock;

class ClassicMap : public OpenCoreMap
{
  public:
    ClassicMap(short id, const string &mapPath) : OpenCoreMap(id, mapPath) {};

    void onEnter() override;
    void onExit() override;

    bool Activate() override;

    BlockInfo &getBlockInfo(int offsetX, int offsetY) override;
};