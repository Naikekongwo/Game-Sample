#include "OpenCore/OpenCore.hpp"
#include <SDL2/SDL_keycode.h>
#include <memory>
#include <optional>

MapExplorer::MapExplorer(const string &id, short layer)
    : UIElement(id, layer, nullptr)
{

    renderRangeX = *OpenCoreManagers::SetManager.getRenderWidth();
    renderRangeY = *OpenCoreManagers::SetManager.getRenderHeight();
    LOG("A");
}

void MapExplorer::onExit()
{
    // OpenEngine::getInstance().getServerWorldController()->enabled(false);
}

void MapExplorer::onEnter()
{

    if (status == MapExpStatus::Creating)
    {
        initComponents();
        status = MapExpStatus::Ready;
        LOG("B");
    }
}

void MapExplorer::Draw()
{
    LOG("C");
    if (status != MapExpStatus::Ready)
        return;

    if (m_wrdController == nullptr)
        return;

    // <新的自绘制逻辑>

    LOG("ControllerStatus {}", (m_wrdController == nullptr) ? 0 : 1);

    // 首先检查地图状态
    if (VState->getAlpha() > 0.0f && m_wrdController->isMapReady())
    {

        // 不空且当前地图准备好了才允许绘图
        // auto &player = Entities.at(1);
        // auto &pProperties = player->getPhysicalProperties();

        // Vec3 Position = pProperties.getPosition();

        // auto map_width = mapManager->getMapWidth();
        // auto map_height = mapManager->getMapHeight();

        // if ((Position.x < 0 or Position.x > map_width) or
        //     (Position.y > map_height or Position.y < 0))
        // {
        //     Position = {0, 0, 0};
        //     pProperties.setPosition(Position);
        //     Vec3 Speed{0, 0, 0};
        //     pProperties.setSpeed(Speed);
        // }
        // //先放弃实体渲染

        auto cameraProp =
            m_wrdController->queryPhysicalProp(m_focusEntityIndex);
        if (cameraProp == std::nullopt)
        {
            LOG("MapExplorer::Draw() 重心不存在");
            return;
        }

        auto Position = cameraProp->getPosition();

        int center_x = Position.x + 0.5f;
        int center_y = Position.y + 0.5f;

        LOG("{} {}", center_x, center_y);

        float offsetX = Position.x - center_x;
        float offsetY = Position.y - center_y;

        auto left_border = (renderRangeX / 2);
        auto up_border = (renderRangeY - 1) / 2;

        for (int y = -up_border; y <= up_border; ++y)
        {
            for (int x = -left_border; x <= left_border; ++x)
            {
                int gx = x + center_x;
                int gy = y + center_y;

                auto bInfo = m_wrdController->queryBlockInfo(gx, gy);

                if (bInfo == std::nullopt)
                    continue;

                // 渲染时加入 sub-grid 偏移
                tileRenderer->setPosition(
                    (x - offsetX) * widthFactor + 0.5f,
                    (y - offsetY + Position.z) * heightFactor + 0.5f);

                tileRenderer->setTransparency(1.0f);
                tileRenderer->getVisualState()->frameIndex = bInfo->Terrain;
                tileRenderer->setTileType(TileType::Terrain);
                tileRenderer->setTileID(bInfo->Terrain);
                tileRenderer->Draw();
            }
        }
        // if (Entities.contains(1))
        // {

        //     Entities.at(1)->Draw(Position.x, Position.y);
        // }
        // 实体不应当拥有渲染方法
    }
    else
    {
        if (!m_wrdController->isMapReady())
            m_wrdController->initMap();
        LOG("WorldController::Failed! the map is not ready!");
    }
    LOG("d");
    // <新的自绘制逻辑>
    // World->Draw();
}

void MapExplorer::onUpdate(float totalTime)
{
    if (status == MapExpStatus::Creating)
    {
        onEnter();
    }
    else
    {
    }
}

void MapExplorer::handlEvents(SDL_Event &event, float totalTime)
{
    if (m_wrdController == nullptr)
        return;

    auto World = m_wrdController;

    if (status == MapExpStatus::Ready && World->isVisible())
    {
        auto pProperties = World->queryPhysicalProp(ENTITY_PLAYER_ID);
        if (pProperties == std::nullopt)
        {
            LOG("MapExplorer::handlEvents empty player properties.\n");
            return;
        }
        if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_w:
            {
                Vec3 speed{0, -5, 0};
                pProperties->addSpeed(speed);
                break;
            }
            case SDLK_s:
            {
                Vec3 speed{0, 5, 0};
                pProperties->addSpeed(speed);
                break;
            }
            case SDLK_a:
            {
                Vec3 speed{-5, 0, 0};
                pProperties->addSpeed(speed);
                break;
            }
            case SDLK_d:
            {
                Vec3 speed{5, 0, 0};
                pProperties->addSpeed(speed);
                break;
            }
            case SDLK_SPACE:
            {
                LOG("Fuck");
                Vec3 speed{0, 0, 5};
                pProperties->addSpeed(speed);
                break;
            }
            default:
                break;
            }
        }
    }
}

bool MapExplorer::setWorldController(WorldController *wrdController)
{
    if (!wrdController)
        return false;

    this->m_wrdController = wrdController;

    return true;
}

void MapExplorer::setExplorerViewPort(ViewportType vType)
{
    this->vType = vType;

    renderRangeX = OpenCoreManagers::SetManager.getRenderWidth().use_count();
    renderRangeY = OpenCoreManagers::SetManager.getRenderHeight().use_count();

    if (vType != ViewportType::Fullscreen && vType != ViewportType::Free)
    {
        renderRangeX *= 0.5;
    }
}

void MapExplorer::initComponents()
{
    if (tileRenderer)
        return;

    // 创建Tile的渲染器
    tileRenderer = std::make_unique<Tile>();
    tileRenderer->Configure()
        .Parent(nullptr)
        .Anchor(AnchorPoint::Center)
        .Alpha(0.0f);
    LOG("MapExplorer:: tileRenderer created successfully.");
    tileRenderer->onEnter();

    // 初始化Tile的大小
    widthFactor = 1.0f / renderRangeX;
    heightFactor = 1.0f / renderRangeY;

    tileRenderer->setScale(widthFactor, heightFactor);
}
