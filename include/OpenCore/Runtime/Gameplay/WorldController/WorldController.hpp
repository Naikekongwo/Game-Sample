#pragma once

/**
 * @file WorldController.hpp
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

#include "OpenCore/Core/Macros.hpp"

#include <cstdint>
#include <memory>
#include <optional>
#include <queue>
#include <unordered_map>

using std::optional;
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
class WorldController
{
  public:
    void onUpdate(float totalTime);
    void onEnter();

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
     * @deprecated 不合适
     * @return true
     * @return false
     */
    bool isVisible() const noexcept
    {
        return status == WorldControllerStatus::Visible;
    }

    /**
     * @brief 获取对应ID的实体的物理信息
     * @brief 只允许返回副本，不允许返回引用，防止越界修改
     * @return PhysicalProperties
     */
    PhysicalProperties *queryPhysicalProp(short EntityIndex);

    /**
     * @brief 获取地图管理器的状态
     *
     * @return true
     * @return false
     */
    bool isMapReady() const noexcept { return mapManager->isReady(); }

    /**
     * @brief 查询BLOCK信息
     *
     * @param x
     * @param y
     * @return optional<BlockInfo>
     */
    optional<BlockInfo> queryBlockInfo(int gx, int gy);

    /**
     * @brief 初始化当前地图
     *
     * @return true
     * @return false
     */
    bool initMap() { return mapManager->initCurrentMap(); }

    Entity *getEntityByID(short id);
    void getEntities(vector<Entity *> &list);

    BackPtr getBackpackByID(short id);
    BackPtr getBackpackByEntityID(short id);

    bool regMovement(short entityID, Vec3 Speed);

    void EnableUpdate() { status = WorldControllerStatus::Visible; }

  private:
    /**
     * @brief 构建并初始化 MapManager对象
     *
     */
    bool generateMapManager();

    /**
     * @brief 初始化主角的实体
     *
     */
    bool generateTheMan();

  protected:
    /**
     * @brief 世界控制器的地图管理器
     * @details
     * 这个成员一定是是用独占指针来管理的，只有世界控制器才有地图的访问权限
     */
    unique_ptr<MapManager> mapManager;

    queue<ItemExchangeRecord> Market;
    unordered_map<short, BackPtr> containers;
    int BackpackCounts = 1;

    unordered_map<short, EntityPtr> Entities;

    WorldControllerStatus status = WorldControllerStatus::Registered;
};