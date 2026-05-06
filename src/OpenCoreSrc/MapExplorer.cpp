#include "OpenCore/Runtime/Graphics/UI/MapExplorer.hpp"
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Animation/IAnimation.hpp"
#include "OpenCore/Runtime/Graphics/Sprite/HealthBar.hpp"
#include "OpenCore/Runtime/Graphics/UI/ItemContainer.hpp"
#include <SDL2/SDL_events.h>
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

        // <TODO>
        auto chao = m_wrdController->queryPhysicalProp(2);
        if (chao != std::nullopt)
        {
            Vec3 pos{2, 0, 0};
            chao->setPosition(pos);
        }

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

        for (int y = -up_border - 1; y <= up_border + 1; ++y)
        {
            for (int x = -left_border; x <= left_border; ++x)
            {
                int gx = x + center_x;
                int gy = y + center_y;

                auto bInfo = m_wrdController->queryBlockInfo(gx, gy);

                if (bInfo == std::nullopt)
                    continue;
                if (bInfo->Terrain == bInfo->STRuct &&
                    bInfo->Terrain == bInfo->Access)
                    continue;

                // <Terrain纹理板块>
                tileRenderer->setPosition(
                    (x - offsetX) * widthFactor + 0.5f + viewportX,
                    (y - offsetY + Position.z) * heightFactor + 0.5f);

                tileRenderer->setTransparency(1.0f);
                tileRenderer->getVisualState()->frameIndex = bInfo->Terrain;
                tileRenderer->setTileType(TileType::Terrain);
                tileRenderer->setTileID(bInfo->Terrain);
                tileRenderer->Draw();
                // <Terrain纹理板块>

                tileRenderer->setTileType(TileType::Terrain);
                tileRenderer->setTileID(bInfo->STRuct);
                tileRenderer->Draw();
            }
        }

        vector<Entity *> Entities;

        m_wrdController->getEntities(Entities);

        for (auto ptr : Entities)
        {
            auto ePos = ptr->getPhysicalProperties().getPosition();
            Vec3 absPos;

            absPos.x = (ePos.x - Position.x) * widthFactor + viewportX + 0.5f;
            absPos.y = (ePos.y - Position.y) * heightFactor + 0.5f;
            absPos.z = 0;

            ptr->Draw(absPos);
        }

        m_itemContainer->Draw();

        m_healthbar->setHealth(
            m_wrdController->getEntityByID(m_focusEntityIndex)
                ->getHealthPercent());
        m_healthbar->Draw();

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

    if (event.type != SDL_KEYDOWN && event.type != SDL_KEYUP)
    {
        return;
    }

    if (vType == ViewportType::Fullscreen || vType == ViewportType::LeftHalf)
    {
        switch (event.key.keysym.sym)
        {
        case SDLK_w:
        {
            m_wrdController->regMovement(m_focusEntityIndex, Vec3(0, -2, 0));
            break;
        }
        case SDLK_a:
        {
            m_wrdController->regMovement(m_focusEntityIndex, Vec3(-2, 0, 0));
            break;
        }
        case SDLK_d:
        {
            m_wrdController->regMovement(m_focusEntityIndex, Vec3(2, 0, 0));
            break;
        }
        case SDLK_s:
        {
            m_wrdController->regMovement(m_focusEntityIndex, Vec3(0, 2, 0));
            break;
        }
        default:
            break;
        }
    }
    else
    {
        switch (event.key.keysym.sym)
        {
        case SDLK_UP:
        {
            m_wrdController->regMovement(m_focusEntityIndex, Vec3(0, -2, 0));
            break;
        }
        case SDLK_LEFT:
        {
            m_wrdController->regMovement(m_focusEntityIndex, Vec3(-2, 0, 0));
            break;
        }
        case SDLK_RIGHT:
        {
            m_wrdController->regMovement(m_focusEntityIndex, Vec3(2, 0, 0));
            break;
        }
        case SDLK_DOWN:
        {
            m_wrdController->regMovement(m_focusEntityIndex, Vec3(0, 2, 0));
            break;
        }
        default:
            break;
        }
    }
}

bool MapExplorer::setWorldController(WorldController *wrdController)
{
    if (!wrdController)
        return false;

    this->m_wrdController = wrdController;

    wrdController->EnableUpdate();

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

    auto thing = m_wrdController;

    m_itemContainer->Configure()
        .Parent(this)
        .Anchor(AnchorPoint::BottomCenter)
        .Scale(0.533f, 0.1185f)
        .Posite(0.5f, 0.95f)
        .Alpha(1.0f)
        .Follow(2);

    m_itemContainer->setBackpack(
        m_wrdController->getBackpackByEntityID(m_focusEntityIndex));
    m_itemContainer->setIndexRange(std::make_pair(0, 7));
    m_itemContainer->onEnter();

    // 初始化生命值

    m_healthbar = std::make_unique<HealthBar>(
        "map_hp", 99, MakeTexture(1, 2, ui_img_healthbar));

    m_healthbar->onEnter();
    m_healthbar->Configure()
        .Parent(this)
        .Anchor(AnchorPoint::TopRight)
        .Scale(0.0f, 0.16f)
        .Posite(0.95f, 0.055f);

    LOG("物品栏创建成功");
}
