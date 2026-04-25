#include "OpenCore/OpenCore.hpp"
#include <SDL2/SDL_keycode.h>
#include <memory>
#include <optional>

MapExplorer::MapExplorer(const string &id, short layer)
    : UIElement(id, layer, nullptr)
{

    renderRangeX = *OpenCoreManagers::SetManager.getRenderWidth();
    renderRangeY = *OpenCoreManagers::SetManager.getRenderHeight();
    LOG("初始化成功");
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
        LOG("状态从创建态更新到准备态");
    }
}

void MapExplorer::Draw()
{
    if (status != MapExpStatus::Ready)
        return;

    if (m_wrdController == nullptr)
        return;

    // <新的自绘制逻辑>

    // 首先检查地图状态
    if (VState->getAlpha() > 0.0f && m_wrdController->isMapReady())
    {

        auto cameraProp =
            m_wrdController->queryPhysicalProp(m_focusEntityIndex);
        if (cameraProp == std::nullopt)
        {
            LOG("渲染的焦点实体物理信息并不存在");
            return;
        }

        auto Position = cameraProp->getPosition();

        int center_x = Position.x + 0.5f;
        int center_y = Position.y + 0.5f;

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
    }
    else
    {
        if (!m_wrdController->isMapReady())
            m_wrdController->initMap();
        LOG("世界控制器尚未准备完毕");
    }
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

    // 暂时为空
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

    renderRangeX = *OpenCoreManagers::SetManager.getRenderWidth();
    renderRangeY = *OpenCoreManagers::SetManager.getRenderHeight();

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
    LOG("地图单位渲染器创建成功");
    tileRenderer->onEnter();

    // 初始化Tile的大小
    widthFactor = 1.0f / renderRangeX;
    heightFactor = 1.0f / renderRangeY;

    tileRenderer->setScale(widthFactor, heightFactor);
}
