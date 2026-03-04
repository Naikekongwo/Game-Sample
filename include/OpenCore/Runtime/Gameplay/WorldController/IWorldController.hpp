#pragma once

// IWorldController.hpp
// 世界控制器的基类

#include "OpenCore/Runtime/Gameplay/Entity/Entity.hpp"
#include "OpenCore/Runtime/Gameplay/Physics/PhysicalProperties.h"
#include "OpenCore/World/Map/Manager/MapManager.hpp"
#include <cstdint>
#include <memory>

class Tile;

class IWorldController
{
  public:
    void onUpdate(float totalTime);
    void onEnter();
    void Draw();

    void enablePlayer(bool enabled = true) { renderPlayer = enabled; }

    // 地图管理器
    unique_ptr<MapManager> mapManager;
    uint8_t renderRangeX = 16;
    uint8_t renderRangeY = 9;
    uint8_t left_border = 7;
    uint8_t up_border = 4;

    // 是否渲染人物
    bool renderPlayer = true;
    unique_ptr<Entity> player;

    // 渲染管线
    unique_ptr<Tile> tileRenderer;
    float widthFactor = 1.0f;
    float heightFactor = 1.0f;
};