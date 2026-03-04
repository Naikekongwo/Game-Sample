#include "OpenCore/OpenCore.hpp"
#include <memory>

void IWorldController::onEnter()
{
    if (!tileRenderer)
    {
        // 创建Tile的渲染器
        tileRenderer = std::make_unique<Tile>();
        tileRenderer->Configure()
            .Parent(nullptr)
            .Anchor(AnchorPoint::Center)
            .Alpha(0.0f);

        tileRenderer->onEnter();
    }

    if (!mapManager)
    {
        // 创建地图管理器
        mapManager = std::make_unique<MapManager>();

        renderRangeX = *OpenCoreManagers::SetManager.getRenderWidth();
        renderRangeY = *OpenCoreManagers::SetManager.getRenderHeight();

        left_border = (renderRangeX - 2) / 2 + 1;
        up_border = (renderRangeY - 1) / 2 + 1;

        widthFactor = 1.0f / renderRangeX;
        heightFactor = 1.0f / renderRangeY;

        mapManager->registerClassicMap(1, "maps/test_circle_radius.ocmp");
        mapManager->setCurrentID(1);

        tileRenderer->setScale(widthFactor, heightFactor);
    }

    if (!Entities.contains(1))
    {
        auto entityreg = Gameplay::EntityReg;
        auto player = entityreg.createEntity(1);

        player->enableDrawer(true);
        Entities[1] = std::move(player);
    }
}

void IWorldController::Draw()
{
    // 首先检查地图状态
    if (!mapManager->empty() && mapManager->legal())
    {
        // 不空且当前地图准备好了才允许绘图
        auto &player = Entities.at(1);
        auto &pProperties = player->getPhysicalProperties();

        Vec3 Position = pProperties.getPosition();

        auto map_width = mapManager->getMapWidth();
        auto map_height = mapManager->getMapHeight();

        if ((Position.x < 0 or Position.x > map_width) or
            (Position.y > map_height or Position.y < 0))
        {
            Position = {0, 0, 0};
            pProperties.setPosition(Position);
            Vec3 Speed{0, 0, 0};
            pProperties.setSpeed(Speed);
        }

        int center_x = Position.x + 0.5f;
        int center_y = Position.y + 0.5f;

        Console_Log("%d %d", center_x, center_y);

        float offsetX = Position.x - center_x;
        float offsetY = Position.y - center_y;

        for (int y = -up_border; y <= up_border; ++y)
        {
            for (int x = -left_border; x <= left_border; ++x)
            {
                int gx = x + center_x;
                int gy = y + center_y;

                BlockInfo &info = mapManager->getBlockInfo(gx, gy);

                // 渲染时加入 sub-grid 偏移
                tileRenderer->setPosition(
                    (x - offsetX) * widthFactor + 0.5f,
                    (y - offsetY + Position.z) * heightFactor + 0.5f);

                tileRenderer->setTransparency(1.0f);
                tileRenderer->getVisualState()->frameIndex = info.Terrain;
                tileRenderer->setTileType(TileType::Terrain);
                tileRenderer->setTileID(info.Terrain);
                tileRenderer->Draw();
            }

            if (Entities.contains(1))
            {

                Entities.at(1)->Draw(Position.x, Position.y);
            }
        }
    }
    else
    {
        Console_Log("WorldController::Failed! the map is not ready!");
    }
}

// void IWorldController::registerMap(short id, string path)
// {
//     if (mapManager)
//     {
//         mapManager->registerClassicMap(id, path);
//     }
//     else
//     {
//         Console_Log("WorldController::Failed to register map %d, patth %s",
//         id,
//                     path.c_str());
//     }
// }

void IWorldController::onUpdate(float totalTime)
{
    for (auto &entry : Entities)
    {
        entry.second->onUpdate(totalTime);
    }

    // 检查地图状态
    if (!mapManager->empty() && !mapManager->legal())
    {
        // 不空但不合法
        mapManager->ActivateMap();
    }
}