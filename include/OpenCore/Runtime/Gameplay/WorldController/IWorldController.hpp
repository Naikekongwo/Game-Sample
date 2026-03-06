#pragma once

// IWorldController.hpp
// 世界控制器的基类

#include "OpenCore/Runtime/Gameplay/Backpack/Backpack.hpp"
#include "OpenCore/Runtime/Gameplay/Entity/Entity.hpp"
#include "OpenCore/Runtime/Gameplay/Entity/EntityRegister.hpp"
#include "OpenCore/Runtime/Gameplay/Physics/PhysicalProperties.h"
#include "OpenCore/World/Map/Manager/MapManager.hpp"

#include <cstdint>
#include <memory>
#include <queue>
#include <unordered_map>

using std::queue;
using std::unordered_map;

class Tile;

enum class WorldControllerStatus
{
    Registered,
    Ready,
    Visible
};

class IWorldController
{
  public:
    static IWorldController &getInstance();

    void onUpdate(float totalTime);
    void onEnter();
    void Draw();

    void enabled(bool Visibility = true);

    // 记录一次物品交换
    void RecordItemExchange(const ItemExchangeRecord &record)
    {
        Market.push(record);
    }

    void enablePlayer(bool enabled = true) { renderPlayer = enabled; }

    PhysicalProperties &getProperties()
    {
        return Entities.at(1)->getPhysicalProperties();
    }

    BackPtr createBackpack(short backpackCapacity)
    {
        BackPtr backpack = std::make_shared<Backpack>(backpackCapacity);

        backpack->setBackpackID(BackpackCounts);
        BackpackCounts++;

        containers[backpack->getBackpackID()] = backpack;

        return backpack;
    }

    bool isVisible() const noexcept
    {
        return status == WorldControllerStatus::Visible;
    }

  protected:
    // 地图管理器
    unique_ptr<MapManager> mapManager;
    uint8_t renderRangeX = 16;
    uint8_t renderRangeY = 9;
    uint8_t left_border = 7;
    uint8_t up_border = 4;

    // 物品管理器
    queue<ItemExchangeRecord> Market;
    unordered_map<short, BackPtr> containers;
    int BackpackCounts = 1;

    // 是否渲染人物
    bool renderPlayer = true;
    unordered_map<short, EntityPtr> Entities;

    // 状态
    WorldControllerStatus status = WorldControllerStatus::Registered;

    // 渲染管线
    unique_ptr<Tile> tileRenderer;
    float widthFactor = 1.0f;
    float heightFactor = 1.0f;
};