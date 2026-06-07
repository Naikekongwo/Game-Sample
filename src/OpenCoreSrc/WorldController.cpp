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

    LOG("地图管理器初始化成功");

    return (mapManager != nullptr);
}

bool WorldController::spawnMapEntities()
{
    if (!mapManager || !mapManager->isReady())
        return false;

    uint16_t mapWidth = mapManager->getMapWidth();
    uint16_t mapHeight = mapManager->getMapHeight();

    for (int x = 0; x < mapWidth; x++)
    {
        for (int y = 0; y < mapHeight; y++)
        {
            BlockInfo bf = mapManager->getBlockInfo(x, y);
            if (bf.Entity == 0 || Entities.contains(bf.Entity))
                continue;

            EntityPtr newEntity =
                Gameplay::EntityReg.getInstance().createEntity(bf.Entity);
            if (!newEntity)
                continue;

            auto eInfo =
                Gameplay::EntityReg.getInstance().getEntityInfo(bf.Entity);

            // Entity 以 BottomCenter 锚点，站在标记方块上
            // X = 网格整数（左右居中），Y = 标记方块底部（网格 + 0.5）
            Vec3 pos{x * 1.0f, y + 0.5f, 0};
            newEntity->enableDrawer(true);
            newEntity->getPhysicalProperties().setPosition(pos);

            // TypeID >= 10 的实体占据其脚下的方块（以标记方块为中心）
            // 碰撞高度按 collisionHeightScale 缩放到下半部分
            if (bf.Entity >= 10)
            {
                uint8_t tw = static_cast<uint8_t>(eInfo.widthFactor);
                uint8_t th = static_cast<uint8_t>(eInfo.heightFactor);
                uint8_t effH =
                    newEntity->getPhysicalProperties().getEffectiveTileHeight();

                int startX = x - (tw - 1) / 2;
                int startY = y; // 标记方块行 = 实体底部行

                LOG("[SPAWN] Entity {} at marker({},{}), tw={} th={} effH={}, "
                    "startX={} startY={}",
                    bf.Entity, x, y, tw, th, effH, startX, startY);

                for (int dy = 0; dy < effH; ++dy)
                {
                    for (int dx = 0; dx < tw; ++dx)
                    {
                        int gx = startX + dx;
                        int gy = startY - dy; // 从底部行向上占据
                        if (gx >= 0 && gx < mapWidth && gy >= 0 &&
                            gy < mapHeight)
                        {
                            LOG("[SPAWN]   mark Access=0 at ({}, {})", gx, gy);
                            mapManager->getBlockInfo(gx, gy).Access = 0;
                        }
                    }
                }
            }

            Entities[bf.Entity] = std::move(newEntity);
        }
    }

    return true;
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
        // 控制单元: 中级 + 高级
        player->getBackpack()->addItem(4, 1);
        player->getBackpack()->addItem(5, 1);
        // 反应堆: 低级 + 中级
        player->getBackpack()->addItem(6, 1);
        player->getBackpack()->addItem(7, 1);
        // 储水罐: 中级 + 高级
        player->getBackpack()->addItem(10, 1);
        player->getBackpack()->addItem(11, 1);
        // 两瓶满水
        player->getBackpack()->addItem(2, 1);
        player->getBackpack()->addItem(2, 1);
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

bool WorldController::pushHomelessItem(short backpackID, short backpackIndex)
{
    // 首先验证背包有效性
    if (!containers.contains(backpackID))
        return false;
    if (containers.at(backpackID)->getCapacity() <= backpackIndex)
        return false;

    auto itemOpt = containers.at(backpackID)->getItem(backpackIndex);
    if (!itemOpt.has_value() || !itemOpt->item.has_value())
        return false;

    ItemExchangeRecord record{itemOpt.value(), backpackID, 0, false};

    m_homelessItem = record;

    // 清空原槽位
    containers.at(backpackID)->removeItem(backpackIndex);
    return true;
}

optional<ItemInstance> WorldController::popHomelessItem()
{
    if (!m_homelessItem.has_value())
        return std::nullopt;

    ItemInstance instance = m_homelessItem->instance;
    m_homelessItem = std::nullopt;
    return instance;
}

optional<ItemInfo> WorldController::queryHomelessItemInfo()
{
    if (m_homelessItem.has_value())
    {
        return m_homelessItem->instance.item->getItemInfo();
    }

    return std::nullopt;
}

void WorldController::giveUpHomelessItem()
{
    if (m_homelessItem.has_value())
    {
        if (containers.contains(m_homelessItem->srcBackpackID))
        {
            containers.at(m_homelessItem->srcBackpackID)
                ->addItem(m_homelessItem->instance.item->getTypeID(),
                          m_homelessItem->instance.count);
        }
    }

    m_homelessItem = std::nullopt;
}