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

    bool onEnter() override;
    void onExit() override;

    BlockInfo &getBlockInfo(int offsetX, int offsetY) override;

  private:
    vector<BlockInfo> Data;
};