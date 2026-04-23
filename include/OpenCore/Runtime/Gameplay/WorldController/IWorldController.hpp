#pragma once

/**
 * @file IWorldController.hpp
 * @author Naikekongwo
 * @brief 世界控制器的头文件
 * @version 0.1
 * @date 2026-04-23
 *
 * @copyright Copyright (c) 2026
 *
 */

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

/**
 * @brief 世界控制器类
 *
 */
class IWorldController
{
  public:
    static IWorldController &getInstance();

    void onUpdate(float totalTime);
    void onEnter();
    void Draw();

    void enabled(bool Visibility = true);

    /**
     * @brief 记录物品的交换
     *
     * @param record
     */
    void RecordItemExchange(const ItemExchangeRecord &record)
    {
        Market.push(record);
    }

    /**
     * @brief 是否启用主角玩家
     *
     * @param enabled
     */
    void enablePlayer(bool enabled = true) { renderPlayer = enabled; }

    /**
     * @brief 获取玩家的物理信息
     *
     * @return PhysicalProperties&
     */
    PhysicalProperties &getProperties()
    {
        return Entities.at(1)->getPhysicalProperties();
    }

    /**
     * @brief 创建背包（按照容量）
     *
     * @param backpackCapacity
     * @return BackPtr
     */
    BackPtr createBackpack(short backpackCapacity)
    {
        BackPtr backpack = std::make_shared<Backpack>(backpackCapacity);

        backpack->setBackpackID(BackpackCounts);
        BackpackCounts++;

        containers[backpack->getBackpackID()] = backpack;

        return backpack;
    }

    /**
     * @brief 设置可见性
     *
     * @return true
     * @return false
     */
    bool isVisible() const noexcept
    {
        return status == WorldControllerStatus::Visible;
    }

  private:
    /**
     * @brief 构建并初始化 MapManager对象
     *
     */
    bool generateMapManager();

    /**
     * @brief 构建并初始化地图块渲染工具 tileRenderer 对象
     *
     */
    bool generateTileRenderer();

    /**
     * @brief 初始化主角的实体
     *
     */
    bool generateTheMan();

  protected:
    unique_ptr<MapManager> mapManager;
    uint8_t renderRangeX = 16;
    uint8_t renderRangeY = 9;
    uint8_t left_border = 7;
    uint8_t up_border = 4;

    queue<ItemExchangeRecord> Market;
    unordered_map<short, BackPtr> containers;
    int BackpackCounts = 1;

    bool renderPlayer = true;
    unordered_map<short, EntityPtr> Entities;

    WorldControllerStatus status = WorldControllerStatus::Registered;

    unique_ptr<Tile> tileRenderer;
    float widthFactor = 1.0f;
    float heightFactor = 1.0f;
};
