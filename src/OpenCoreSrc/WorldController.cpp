#include "OpenCore/Runtime/Gameplay/WorldController/WorldController.hpp"
#include "OpenCore/Core/Math/OpenCore_Vec3.hpp"
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Gameplay/Backpack/Backpack.hpp"
#include "OpenCore/Runtime/Gameplay/Entity/Entity.hpp"
#include "OpenCore/Runtime/Gameplay/Entity/EntityRegister.hpp"
#include "OpenCore/Runtime/Gameplay/Physics/PhysicalProperties.h"
#include <cstdint>
#include <memory>
#include <optional>

void WorldController::onEnter()
{
    if (status == WorldControllerStatus::Registered)
    {
        bool result = true;

        // 连续初始化
        result &= generateMapManager();
        result &= generateTheMan();

        if (!result)
            LOG("世界控制器在初始化时出现了问题");

        // 清除悬空状态
        m_homelessItem = std::nullopt;

        status = WorldControllerStatus::Ready;
    }
}

bool WorldController::generateMapManager()
{
    if (mapManager != nullptr)
        return true;

    mapManager = std::make_unique<MapManager>();

    // 注册一个默认的地图文件
    mapManager->loadClassicMap(1, "maps/test_newTemple.ocmp");
    mapManager->setCurrentID(1);

    uint16_t mapWidth = 0;
    uint16_t mapHeight = 0;

    mapWidth = mapManager->getMapWidth();
    mapHeight = mapManager->getMapHeight();

    LOG("ENTITY ID: {}", mapWidth);

    for (int x = 0; x < mapWidth; x++)
    {
        for (int y = 0; y < mapHeight; y++)
        {
            BlockInfo bf = mapManager->getBlockInfo(x, y);
            if (bf.Entity != 0)
            {
                // 生成Entity
                EntityPtr newEntity =
                    Gameplay::EntityReg.getInstance().createEntity(bf.Entity);
                Vec3 pos{x * 1.0f, y * 1.0f, 0};
                newEntity->enableDrawer(true);
                newEntity->getPhysicalProperties().setPosition(pos);
                Entities[bf.Entity] = std::move(newEntity);
            }
        }
    }

    LOG("地图管理器初始化成功");

    return (mapManager != nullptr);
}

bool WorldController::generateTheMan()
{
    if (Entities.contains(1))
        return true;
    else
    {
        auto &entityreg = Gameplay::EntityReg.getInstance();
        auto player = entityreg.createEntity(1);

        player->enableDrawer(true);
        Entities[1] = std::move(player);
    }

    if (Entities.contains(2))
        return true;
    else
    {
        auto &entityreg = Gameplay::EntityReg.getInstance();
        auto player = entityreg.createEntity(2);

        player->enableDrawer(true);
        Entities[2] = std::move(player);
    }

    return Entities.contains(1);
}

void WorldController::onUpdate(float totalTime)
{
    // 可见才更新，否则不会更新
    if (status == WorldControllerStatus::Visible)
    {
        // 实体状态更新
        for (auto &entry : Entities)
        {
            entry.second->onUpdate(totalTime);
        }

        // 处理交易数据

        if (!Market.empty())
        {
            for (auto &entry : containers)
            {
                auto record = Market.front();

                entry.second->onUpdate(record, totalTime);
            }

            if (Market.front().dealed == true)
                Market.pop();
        }
        // 交易数据处理完毕

        // 检查地图状态
        if (!mapManager->isReady())
        {
            // 不空但不合法
            mapManager->initCurrentMap();
        }
    }
}

PhysicalProperties *WorldController::queryPhysicalProp(short EntityIndex)
{
    if (Entities.contains(EntityIndex))
    {
        return &Entities.at(EntityIndex)->getPhysicalProperties();
    }

    return nullptr;
}

optional<BlockInfo> WorldController::queryBlockInfo(int gx, int gy)
{
    if (mapManager->isReady())
    {
        return mapManager->getBlockInfo(gx, gy);
    }

    return std::nullopt;
}

Entity *WorldController::getEntityByID(short id)
{
    if (!Entities.contains(id))
        return nullptr;

    return Entities.at(id).get();
}

void WorldController::getEntities(vector<Entity *> &list)
{
    list.clear();
    list.reserve(Entities.size());
    for (const auto &[id, ePtr] : Entities)
    {
        list.push_back(ePtr.get());
    }
}

BackPtr WorldController::getBackpackByEntityID(short id)
{
    auto entity = getEntityByID(id);

    if (!entity)
    {
        LOG("尝试获取的实体对象并不存在, ID {}", id);
        return nullptr;
    }

    auto backpack = entity->getBackpack();
    return backpack;
}

BackPtr WorldController::getBackpackByID(short id)
{
    if (!containers.contains(id))
    {
        LOG("所请求的背包并不存在， ID {}", id);
        return nullptr;
    }

    return containers.at(id);
}

bool WorldController::regMovement(short entityID, Vec3 Speed)
{
    auto entity = getEntityByID(entityID);
    if (!entity)
        return false;

    auto &pProperties = entity->getPhysicalProperties();

    pProperties.setDesiredVelocity(Speed);

    LOG("设置成功");

    return true;
}

// bool WorldController::pushHomelessItem(short backpackID, short backpackIndex)
// {
//     // 首先验证背包有效性
//     if (!containers.contains(backpackID))
//         return false;
//     if (containers.at(backpackID)->getCapacity() <= backpackIndex)
//         return false;

//     return true;
// }

// ItemInstance WorldController::popHomelessItem()
// {
//     return ItemInstance{Gameplay::ItemMgr.createItem(0).value(), 1};
// }