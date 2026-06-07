#include "OpenCore/Runtime/Graphics/UI/MapExplorer.hpp"
#include "Eclipsea/Stage/PurifierStage.hpp"
#include "OpenCore/Core/Macros.hpp"
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Animation/IAnimation.hpp"
#include "OpenCore/Runtime/Graphics/Sprite/HealthBar.hpp"
#include "OpenCore/Runtime/Graphics/UI/ItemContainer.hpp"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_gamecontroller.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <memory>
#include <optional>

#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/Core/Event/ControllerManager.hpp"
#include "OpenCore/Runtime/Graphics/UI/Symbol.hpp"

MapExplorer::MapExplorer(const string &id, short layer)
    : UIElement(id, layer, nullptr)
{
    renderRangeX = *OpenCoreManagers::SetManager.getRenderWidth();
    renderRangeY = *OpenCoreManagers::SetManager.getRenderHeight();

    LOG("初始化成功");
}

void MapExplorer::onExit() {}

void MapExplorer::onEnter()
{

    if (status == MapExpStatus::Creating)
    {
        initComponents();

        // 贴图加载完毕后，从地图方块中生成实体
        m_wrdController->spawnMapEntities();

        // <TODO>
        auto chao = m_wrdController->queryPhysicalProp(2);
        if (chao != nullptr)
        {
            Vec3 pos{2, 4, 0};
            chao->setPosition(pos);
        }

        auto player = m_wrdController->queryPhysicalProp(1);
        if (player != nullptr)
        {
            Vec3 pos{2, 4, 0};
            player->setPosition(pos);
        }

        status = MapExpStatus::Ready;
        LOG("状态从创建态更新到准备态");
    }
}

void MapExplorer::Draw()
{
    if (status != MapExpStatus::Ready || m_wrdController == nullptr)
        return;

    if (VState->getAlpha() <= 0.0f)
    {
        if (!m_wrdController->isMapReady())
            m_wrdController->initMap();
        LOG("世界控制器尚未准备完毕");
        return;
    }

    if (!m_wrdController->isMapReady())
    {
        m_wrdController->initMap();
        LOG("世界控制器尚未准备完毕");
        return;
    }

    auto focusEntity = m_wrdController->getEntityByID(m_focusEntityIndex);

    auto cameraProp =
        focusEntity ? &focusEntity->getPhysicalProperties() : nullptr;
    if (cameraProp == nullptr)
    {
        LOG("渲染的焦点实体物理信息并不存在");
        return;
    }

    auto *renderer = OpenCoreManagers::GFXManager.getInstance().getRenderer();

    SDL_Rect rect{0, 0, 1920, 1080};
    float viewportX = 0.0f;

    switch (vType)
    {
    case ViewportType::LeftHalf:
        viewportX -= 0.25f;
        rect.x = 0;
        rect.y = 0;
        rect.w = 960;
        rect.h = 1080;
        break;
    case ViewportType::RightHalf:
        viewportX += 0.25f;
        rect.x = 960;
        rect.y = 0;
        rect.w = 960;
        rect.h = 1080;
        break;
    default:
        break;
    }

    SDL_RenderSetClipRect(renderer, &rect);

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
            if (bInfo->Terrain == 2 && bInfo->STRuct == 2)
                continue;

            tileRenderer->setPosition(
                (x - offsetX) * widthFactor + 0.5f + viewportX,
                (y - offsetY + Position.z) * heightFactor + 0.5f);

            tileRenderer->setTransparency(1.0f);
            tileRenderer->getVisualState()->frameIndex = bInfo->Terrain;
            tileRenderer->setTileType(TileType::Terrain);
            tileRenderer->setTileID(bInfo->Terrain);
            tileRenderer->Draw();

            tileRenderer->setTileType(TileType::Terrain);
            tileRenderer->setTileID(bInfo->STRuct);
            tileRenderer->Draw();
        }
    }

    vector<Entity *> Entities;
    m_wrdController->getEntities(Entities);

    sort(Entities.begin(), Entities.end(),
         [](Entity *a, Entity *b)
         {
             return a->getPhysicalProperties().getPosition().y <
                    b->getPhysicalProperties().getPosition().y;
         });

    nearbyEntity = nullptr;
    for (auto ptr : Entities)
    {
        auto ePos = ptr->getPhysicalProperties().getPosition();
        Vec3 absPos;

        absPos.x = (ePos.x - Position.x) * widthFactor + viewportX + 0.5f;
        absPos.y = (ePos.y - Position.y) * heightFactor + 0.5f;
        absPos.z = 0;

        ptr->Draw(absPos);

        Vec3 symbolPos = absPos;
        symbolPos.y -= 0.06f;

        if (abs(ePos.x - Position.x) < 2.0f &&
            abs(ePos.y - Position.y) < 2.0f &&
            ePos != cameraProp->getPosition())
        {
            m_symbol->SetSymbolType(SYMBOL_QUESTION);
            m_symbol->setPosition(0.5f + viewportX, 0.27f);
            m_symbol->setScale(0.05f, 0.05f * widthheight);
            m_symbol->Draw();

            nearbyEntity = ptr;
        }
    }

    m_itemContainer->Draw();

    m_healthbar->setHealth(
        m_wrdController->getEntityByID(m_focusEntityIndex)->getHealthHook());
    m_healthbar->Draw();

    // 渲染拖动的物品
    {
        auto homeless = m_wrdController->queryHomelessItemInfo();
        if (homeless.has_value())
        {
            if (!m_pickedUpItem)
            {
                m_pickedUpItem = std::make_unique<ItemSprite>();
                m_pickedUpItem->Configure()
                    .Parent(nullptr)
                    .Anchor(AnchorPoint::Center)
                    .Posite(0.5f, 0.5f)
                    .Scale(0.1f, 0.1f * widthheight);
            }

            auto meta =
                Gameplay::ItemMgr.getTextureMeta(homeless->textureMetaID);
            if (meta.has_value())
            {
                m_pickedUpItem->changeTexture(MakeTexture(
                    meta->texture_cols, meta->texture_rows, meta->textureID));
                m_pickedUpItem->setSubTexture(homeless->texturePosID);
                m_pickedUpItem->getVisualState()->Position[0] = m_mouseX;
                m_pickedUpItem->getVisualState()->Position[1] = m_mouseY;
                m_pickedUpItem->Draw();
            }
        }
        else
        {
            m_pickedUpItem.reset();
        }
    }

    SDL_RenderSetClipRect(renderer, nullptr);
}

void MapExplorer::onUpdate(float totalTime)
{
    if (status == MapExpStatus::Creating)
    {
        onEnter();
    }

    // 人物移动时持续触发手柄低频微震以模拟摩擦感
    if (m_moveUp || m_moveDown || m_moveLeft || m_moveRight)
    {
        ControllerManager::GetInstance().RumblePlayer(0, 0x1000, 0, 80);
    }
}

void MapExplorer::handlEvents(SDL_Event &event, float totalTime)
{
    if (m_wrdController == nullptr)
        return;

    m_itemContainer->handlEvents(event, totalTime);

    // --- 鼠标追踪: 更新拖动物品的位置 ---
    if (event.type == SDL_MOUSEMOTION)
    {
        m_mouseX = static_cast<float>(event.motion.x);
        m_mouseY = static_cast<float>(event.motion.y);
    }

    // --- 鼠标点击: 喝水 ---
    if (event.type == SDL_MOUSEBUTTONDOWN &&
        event.button.button == SDL_BUTTON_LEFT)
    {
        auto homeless = m_wrdController->queryHomelessItemInfo();
        if (homeless.has_value() && homeless->typeID == 2) // 满瓶
        {
            // 检查点击位置是否在屏幕中央区域 (±30%)
            float cx = m_mouseX / 1920.0f;
            float cy = m_mouseY / 1080.0f;
            if (cx >= 0.35f && cx <= 0.65f && cy >= 0.35f && cy <= 0.65f)
            {
                auto player =
                    m_wrdController->getEntityByID(m_focusEntityIndex);
                if (player)
                {
                    m_wrdController->popHomelessItem();
                    player->getBackpack()->addItem(1, 1); // 空瓶放回背包

                    *player->getHealthHook() =
                        std::min(1.0f, *player->getHealthHook() + 0.5f);
                    LOG("喝水: 满瓶已消耗, 恢复50%生命值");
                }
            }
        }
    }

    bool isKeyDown = (event.type == SDL_KEYDOWN);
    bool isCtrlDown = (event.type == SDL_CONTROLLERBUTTONDOWN);

    // --- 键盘输入 ---
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
    {
        if (event.key.repeat)
            return;

        if (vType == ViewportType::Fullscreen ||
            vType == ViewportType::LeftHalf)
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_w:
                m_moveUp = isKeyDown;
                break;
            case SDLK_a:
                m_moveLeft = isKeyDown;
                break;
            case SDLK_d:
                m_moveRight = isKeyDown;
                break;
            case SDLK_s:
                m_moveDown = isKeyDown;
                break;
            case SDLK_RETURN:
            {
                if (nearbyEntity)
                {
                    switch (nearbyEntity->getEntityInfo().EntityTypeID)
                    {
                    case 100:
                    {
                        auto Purifier = std::make_unique<PurifierStage>(
                            OpenEngine::getInstance().getTimer(),
                            OpenEngine::getInstance().getStageController());
                        OpenEngine::getInstance()
                            .getStageController()
                            ->changeStage(std::move(Purifier));
                        break;
                    }
                    default:
                        break;
                    }
                }

                break;
            }
            default:
                return;
            }
        }
        else
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_UP:
                m_moveUp = isKeyDown;
                break;
            case SDLK_LEFT:
                m_moveLeft = isKeyDown;
                break;
            case SDLK_RIGHT:
                m_moveRight = isKeyDown;
                break;
            case SDLK_DOWN:
                m_moveDown = isKeyDown;
                break;
            default:
                return;
            }
        }
    }
    // --- 手柄十字键输入 ---
    else if (event.type == SDL_CONTROLLERBUTTONDOWN ||
             event.type == SDL_CONTROLLERBUTTONUP)
    {
        switch (event.cbutton.button)
        {
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            m_moveUp = isCtrlDown;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            m_moveDown = isCtrlDown;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            m_moveLeft = isCtrlDown;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            m_moveRight = isCtrlDown;
            break;
        default:
            return;
        }
    }
    else
    {
        return;
    }

    // 根据所有当前按住的键计算合成速度
    Vec3 velocity{0, 0, 0};
    if (m_moveUp)
        velocity.y -= 3;
    if (m_moveDown)
        velocity.y += 3;
    if (m_moveLeft)
        velocity.x -= 3;
    if (m_moveRight)
        velocity.x += 3;

    m_wrdController->regMovement(m_focusEntityIndex, velocity);
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

    m_itemContainer->setBackpack(
        m_wrdController->getBackpackByEntityID(m_focusEntityIndex));
    m_itemContainer->setIndexRange(std::make_pair(0, 7));
    m_itemContainer->onEnter();

    // 初始化生命值

    m_healthbar = std::make_unique<HealthBar>(
        "map_hp", 99, MakeTexture(1, 4, ui_img_healthbar));

    m_healthbar->onEnter();
    m_healthbar->Configure()
        .Parent(this)
        .Anchor(AnchorPoint::TopRight)
        .Scale(0.0f, 0.16f)
        .Posite(0.95f, 0.055f);

    m_symbol = std::make_unique<Symbol>("entity_symbol", 99, 2052);

    m_symbol->Configure()
        .Parent(nullptr)
        .Anchor(AnchorPoint::Center)
        .Posite(0.5f, 0.5f)
        .Scale(0.06f, 0.06f);

    LOG("物品栏创建成功");
}
