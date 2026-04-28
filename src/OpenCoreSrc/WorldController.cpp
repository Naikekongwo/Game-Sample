#include "OpenCore/Runtime/Gameplay/WorldController/WorldController.hpp"
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Gameplay/Entity/EntityRegister.hpp"
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

        status = WorldControllerStatus::Ready;
    }
}

bool WorldController::generateMapManager()
{
    if (mapManager != nullptr)
        return true;

    mapManager = std::make_unique<MapManager>();

    // 注册一个默认的地图文件
    mapManager->loadClassicMap(1, "maps/test_circle_radius.ocmp");
    mapManager->setCurrentID(1);

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

// void WorldController::Draw()
// {
//     // 首先检查地图状态
//     if (status == WorldControllerStatus::Visible && mapManager->isReady())
//     {
//         // 不空且当前地图准备好了才允许绘图
//         auto &player = Entities.at(1);
//         auto &pProperties = player->getPhysicalProperties();

//         Vec3 Position = pProperties.getPosition();

//         auto map_width = mapManager->getMapWidth();
//         auto map_height = mapManager->getMapHeight();

//         if ((Position.x < 0 or Position.x > map_width) or
//             (Position.y > map_height or Position.y < 0))
//         {
//             Position = {0, 0, 0};
//             pProperties.setPosition(Position);
//             Vec3 Speed{0, 0, 0};
//             pProperties.setSpeed(Speed);
//         }

//         int center_x = Position.x + 0.5f;
//         int center_y = Position.y + 0.5f;

//         LOG("{} {}", center_x, center_y);

//         float offsetX = Position.x - center_x;
//         float offsetY = Position.y - center_y;

//         for (int y = -up_border; y <= up_border; ++y)
//         {
//             for (int x = -left_border; x <= left_border; ++x)
//             {
//                 int gx = x + center_x;
//                 int gy = y + center_y;

//                 BlockInfo &info = mapManager->getBlockInfo(gx, gy);

//                 // 渲染时加入 sub-grid 偏移
//                 tileRenderer->setPosition(
//                     (x - offsetX) * widthFactor + 0.5f,
//                     (y - offsetY + Position.z) * heightFactor + 0.5f);

//                 tileRenderer->setTransparency(1.0f);
//                 tileRenderer->getVisualState()->frameIndex = info.Terrain;
//                 tileRenderer->setTileType(TileType::Terrain);
//                 tileRenderer->setTileID(info.Terrain);
//                 tileRenderer->Draw();
//             }
//         }
//         if (Entities.contains(1))
//         {

//             Entities.at(1)->Draw(Position.x, Position.y);
//         }
//     }
//     else
//     {
//         if (!mapManager->isReady())
//             mapManager->initCurrentMap();
//         LOG("WorldController::Failed! the map is not ready!");
//     }
// }

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

optional<PhysicalProperties &>
WorldController::queryPhysicalProp(short EntityIndex)
{
    if (Entities.contains(EntityIndex))
    {
        return Entities.at(EntityIndex)->getPhysicalProperties();
    }

    // 为空时返回 nullopt
    return std::nullopt;
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