#include "OpenCore/OpenCore.hpp"
#include <SDL2/SDL_ttf.h>
#include <memory>

void Entity::Configure(const EntityInfo &eInfo)
{
    info = eInfo;
    // 拷贝EntityType
    LOG("实体部署成功，实体类型：{}", info.EntityTypeID);

    backpack =
        OpenEngine::getInstance().getServerWorldController()->createBackpack(
            info.backpackCapacity);

    m_healthPercent = std::make_shared<float>(1.0f);

    pProperties.setTileSize(static_cast<uint8_t>(info.widthFactor),
                            static_cast<uint8_t>(info.heightFactor));

    status = EntityStatus::Ready;
}

void Entity::createRenderer()
{
    // 创建一个角色绘制器
    if (!renderer && info.EntityTypeID != 0)
    {
        m_gridCols = static_cast<uint8_t>(info.widthFactor) * info.frameCount;
        uint8_t gridRows =
            static_cast<uint8_t>(info.heightFactor) * info.frameCount;
        renderer = std::make_unique<Mob>(info.TextureID, m_gridCols, gridRows);
        renderer->Configure().Anchor(AnchorPoint::BottomCenter).Alpha(0.0f);

        widthRelative = 1.0f / *OpenCoreManagers::SetManager.getRenderWidth();
        heightRelative = 1.0f / *OpenCoreManagers::SetManager.getRenderHeight();

        renderer->setTileSize(pProperties.getTileWidth(),
                              pProperties.getTileHeight());
        renderer->Configure().Scale(info.widthFactor * widthRelative, 0.0f);
    }
}

void Entity::Draw(const Vec3 &absPos)
{
    if (!drawable)
        return;

    if (!renderer)
        createRenderer();

    // 更新 world state
    Vec3 Position = pProperties.getPosition();

    // 动画帧选取
    if (!info.animations.empty() && m_currentAnimIndex < info.animations.size())
    {
        const auto &seq = info.animations[m_currentAnimIndex];
        if (!seq.frames.empty())
        {
            int f =
                static_cast<int>(m_animTimer * m_animFPS) % seq.frames.size();
            const auto &frame = seq.frames[f];
            renderer->getVisualState()->frameIndex =
                frame.row * m_gridCols + frame.col;
            uint8_t fw =
                frame.tileW > 0 ? frame.tileW : pProperties.getTileWidth();
            uint8_t fh =
                frame.tileH > 0 ? frame.tileH : pProperties.getTileHeight();
            if (fw != renderer->getTileWidth() ||
                fh != renderer->getTileHeight())
                renderer->setTileSize(fw, fh);
        }
    }
    else
    {
        int tilesPerDir = pProperties.getTileWidth() *
                          pProperties.getTileHeight() * info.frameCount;
        int base = directionBaseIndex(pProperties.getDirection(), tilesPerDir);
        int animFrame =
            static_cast<int>(m_animTimer * m_animFPS) % info.frameCount;
        renderer->getVisualState()->frameIndex =
            info.defaultRow * m_gridCols + info.defaultCol + base + animFrame;
    }

    // ⭐关键：只接收已经算好的 screen 坐标
    renderer->setPosition(absPos.x, absPos.y);

    renderer->setTransparency(1.0f);
    renderer->Draw();
}

void Entity::Draw(float cameraX, float cameraY)
{
    if (drawable)
    {
        if (!renderer)
        {
            // 创建renderer
            createRenderer();
        }

        // 开始绘图
        Vec3 Position = pProperties.getPosition();

        auto renderWidth = *OpenCoreManagers::SetManager.getRenderWidth();
        auto renderHeight = *OpenCoreManagers::SetManager.getRenderHeight();

        renderWidth = (renderWidth - 2) / 2 + 1;
        renderHeight = (renderHeight - 1) / 2 + 1;

        if ((abs(cameraX - Position.x) > renderWidth) or
            (cameraY - Position.y > renderHeight))
        {
            // 超出渲染范围
            return;
        }
        else
        {
            renderer->setPosition(0.5f + (Position.x - cameraX) * widthRelative,
                                  0.5f +
                                      (Position.y - cameraY) * heightRelative);
            renderer->setTransparency(1.0f);
            if (!info.animations.empty() &&
                m_currentAnimIndex < info.animations.size())
            {
                const auto &seq = info.animations[m_currentAnimIndex];
                if (!seq.frames.empty())
                {
                    int f = static_cast<int>(m_animTimer * m_animFPS) %
                            seq.frames.size();
                    const auto &frame = seq.frames[f];
                    renderer->getVisualState()->frameIndex =
                        frame.row * m_gridCols + frame.col;
                    uint8_t fw = frame.tileW > 0 ? frame.tileW
                                                 : pProperties.getTileWidth();
                    uint8_t fh = frame.tileH > 0 ? frame.tileH
                                                 : pProperties.getTileHeight();
                    if (fw != renderer->getTileWidth() ||
                        fh != renderer->getTileHeight())
                        renderer->setTileSize(fw, fh);
                }
            }
            else
            {
                int tilesPerDir = pProperties.getTileWidth() *
                                  pProperties.getTileHeight() * info.frameCount;
                int base =
                    directionBaseIndex(pProperties.getDirection(), tilesPerDir);
                int animFrame =
                    static_cast<int>(m_animTimer * m_animFPS) % info.frameCount;
                renderer->getVisualState()->frameIndex =
                    info.defaultRow * m_gridCols + info.defaultCol + base +
                    animFrame;
            }
            renderer->Draw();
        }
    }
    else if (renderer)
    {
        // 既然不需要渲染，那就不要让renderer占有内存
        renderer.reset();
    }
}

void Entity::onUpdate(float totalTime)
{
    float deltaTime = totalTime - lastTime;
    Vec3 previousPosition = pProperties.getPosition();

    // 刷新
    pProperties.onUpdate(totalTime);

    // 碰撞检测：若新位置不可通行则回退
    Vec3 newPosition = pProperties.getPosition();
    if (!canMoveTo(newPosition))
    {
        pProperties.setPosition(previousPosition);
        pProperties.setSpeed({0, 0, 0});
    }

    // 动画计时
    Vec3 speed = pProperties.getSpeed();
    bool isMoving = (std::abs(speed.x) > 0.01f || std::abs(speed.y) > 0.01f);

    if (!isMoving)
    {
        m_animTimer = 0.0f;
        m_currentAnimIndex = 0;
    }
    else
    {
        uint8_t newAnimIndex = computeAnimIndex();
        if (newAnimIndex != m_currentAnimIndex)
        {
            m_animTimer = 0.0f;
            m_currentAnimIndex = newAnimIndex;
        }
        m_animTimer += deltaTime;
    }

    // 刷新血量

    *m_healthPercent -= 0.05f * deltaTime;

    if (*m_healthPercent <= 0.0f)
        *m_healthPercent = 1.0f;

    lastTime = totalTime;
}

int Entity::directionBaseIndex(Direction dir, int tilesPerDirection)
{
    switch (dir)
    {
    case Direction::Down:
        return 0;
    case Direction::Left:
        return tilesPerDirection;
    case Direction::Right:
        return tilesPerDirection * 2;
    case Direction::Up:
        return tilesPerDirection * 3;
    }
    return 0;
}

bool Entity::canMoveTo(const Vec3 &pos) const
{
    auto wc = OpenEngine::getInstance().getServerWorldController();
    if (!wc)
        return true;

    uint8_t tw = pProperties.getTileWidth();
    uint8_t th = pProperties.getTileHeight();

    int baseX = static_cast<int>(std::floor(pos.x));
    int baseY = static_cast<int>(std::floor(pos.y));

    for (uint8_t dy = 0; dy < th; ++dy)
        for (uint8_t dx = 0; dx < tw; ++dx)
        {
            auto block = wc->queryBlockInfo(baseX + dx, baseY - dy);
            if (block.has_value() &&
                block->Access ==
                    0) // 这里先令等于0为可移动的格子，非0为不可移动的格子，后续可以根据需求细化不同access值代表是否可移动
                return false;
        }

    return true;
}

uint8_t Entity::computeAnimIndex() const
{
    // 仅在 isMoving=true 时调用
    if (info.animations.empty())
        return 0;

    switch (pProperties.getDirection())
    {
    case Direction::Down:
        return 1;
    case Direction::Left:
        return 2;
    case Direction::Right:
        return 3;
    case Direction::Up:
        return 4;
    }
    return 0;
}