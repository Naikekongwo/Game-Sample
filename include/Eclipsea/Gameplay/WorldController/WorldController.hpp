#pragma once

/**
 * @file WorldController.hpp
 * @author Naikekongwo
 * @brief 世界控制器的头文�?
 * @version 0.1
 * @date 2026-04-23
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "Eclipsea/Gameplay/Backpack/Backpack.hpp"
#include "Eclipsea/Gameplay/Backpack/ItemManager.hpp"
#include "Eclipsea/Gameplay/Entity/Entity.hpp"
#include "Eclipsea/Gameplay/Entity/EntityRegister.hpp"
#include "Eclipsea/Gameplay/Physics/PhysicalProperties.h"
#include "Eclipsea/Gameplay/Map/Manager/MapManager.hpp"

#include "Core/Macros.hpp"

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
    // 单例访问器（原引擎由 OpenEngine 持有，现为项目级单例）
    static WorldController &getInstance();

    // ─── 生命周期 ───────────────────────────────────
    void onUpdate(float totalTime);
    void onEnter();

    // ─── 查询接口 ───────────────────────────────────
    PhysicalProperties *queryPhysicalProp(short EntityIndex);
    optional<BlockInfo> queryBlockInfo(int gx, int gy);

    Entity *getEntityByID(short id);
    void getEntities(vector<Entity *> &list);

    BackPtr getBackpackByID(short id);
    BackPtr getBackpackByEntityID(short id);

    bool regMovement(short entityID, Vec3 Speed);

    // ─── 内联工具 ───────────────────────────────────
    /** @brief 记录物品的交�?*/
    void RecordItemExchange(const ItemExchangeRecord &record)
    {
        Market.push(record);
    }

    /** @brief 创建背包（按照容量） */
    BackPtr createBackpack(short backpackCapacity)
    {
        BackPtr backpack = std::make_shared<Backpack>(backpackCapacity);
        backpack->setBackpackID(BackpackCounts);
        BackpackCounts++;
        containers[backpack->getBackpackID()] = backpack;
        return backpack;
    }

    /** @brief 获取地图管理器的状�?*/
    bool isMapReady() const noexcept { return mapManager->isReady(); }

    /** @brief 初始化当前地�?*/
    bool initMap() { return mapManager->initCurrentMap(); }

    /** @brief 从地图方块数据中生成实体（在贴图加载后调用） */
    bool spawnMapEntities();

    /** @deprecated 不合�?*/
    bool isVisible() const noexcept
    {
        return status == WorldControllerStatus::Visible;
    }

    void EnableUpdate() { status = WorldControllerStatus::Visible; }

    bool pushHomelessItem(short backpackID, short backpackIndex);
    optional<ItemInstance> popHomelessItem();
    optional<ItemInfo> queryHomelessItemInfo();
    void giveUpHomelessItem();

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
     * @brief 世界控制器的地图管理�?
     * @details
     * 这个成员一定是是用独占指针来管理的，只有世界控制器才有地图的访问权�?
     */
    unique_ptr<MapManager> mapManager;

    queue<ItemExchangeRecord> Market;
    unordered_map<short, BackPtr> containers;
    int BackpackCounts = 1;

    // 新加入的槽位，用于容纳那些悬空的物品
    optional<ItemExchangeRecord> m_homelessItem = std::nullopt;

    unordered_map<short, EntityPtr> Entities;

    WorldControllerStatus status = WorldControllerStatus::Registered;
};