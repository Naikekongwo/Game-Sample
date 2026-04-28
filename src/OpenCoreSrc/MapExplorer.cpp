#include "OpenCore/Runtime/Graphics/UI/MapExplorer.hpp"
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Animation/IAnimation.hpp"
#include "OpenCore/Runtime/Graphics/UI/ItemContainer.hpp"
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <memory>
#include <optional>

#include "Eclipsea/Eclipsea.hpp"

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

        // <开始绘制>

        SDL_Rect rect{0, 0, 1920, 1080};

        float viewportX = 0.0f;

        switch (vType)
        {
        case ViewportType::LeftHalf:
        {
            viewportX -= 0.25f;
            rect.x = 0;
            rect.y = 0;
            rect.w = 960;
            rect.h = 1080;
            break;
        }
        case ViewportType::RightHalf:
        {
            viewportX += 0.25f;
            rect.x = 960;
            rect.y = 0;
            rect.w = 960;
            rect.h = 1080;
            break;
        }
        default:
            break;
        }

        SDL_RenderSetClipRect(
            OpenCoreManagers::GFXManager.getInstance().getRenderer(), &rect);

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
                    (x - offsetX) * widthFactor + 0.5f + viewportX,
                    (y - offsetY + Position.z) * heightFactor + 0.5f);

                tileRenderer->setTransparency(1.0f);
                tileRenderer->getVisualState()->frameIndex = bInfo->Terrain;
                tileRenderer->setTileType(TileType::Terrain);
                tileRenderer->setTileID(bInfo->Terrain);
                tileRenderer->Draw();
            }
        }

        vector<Entity *> Entities;

        // <TODO>
        auto chao = m_wrdController->queryPhysicalProp(2);
        if (chao != std::nullopt)
        {
            Vec3 pos{2, 0, 0};
            chao->setPosition(pos);
        }

        m_wrdController->getEntities(Entities);

        for (auto ptr : Entities)
        {
            ptr->Draw(Position.x, Position.y, viewportX);
        }

        m_itemContainer->setPosition(0.5f + viewportX, 0.95f);
        m_itemContainer->Draw();

        SDL_RenderSetClipRect(
            OpenCoreManagers::GFXManager.getInstance().getRenderer(), nullptr);
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

        // widthFactor = 1.0f / renderRangeX;
        // heightFactor = 1.0f / renderRangeY;

        // tileRenderer->setScale(widthFactor, heightFactor);
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

    m_itemContainer = UI<ItemContainer>("itemContainer", 99, 2027, 8, 1);
    m_itemContainer->setParentContainer(this);

    m_itemContainer->Configure()
        .Parent(this)
        .Anchor(AnchorPoint::BottomCenter)
        .Scale(0.533f, 0.1185f)
        .Posite(0.5f, 0.95f)
        .Alpha(1.0f)
        .Follow(2);

    m_itemContainer->setSize(1, 8);
    m_itemContainer->onEnter();

    LOG("物品栏创建成功");
}
