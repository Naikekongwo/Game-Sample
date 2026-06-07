#include "OpenCore/OpenCore.hpp"
#include <SDL2/SDL_ttf.h>
#include <memory>

void Entity::Configure(const EntityInfo &eInfo)
{
    info = eInfo;
    LOG("实体部署成功，实体类型：{}", info.EntityTypeID);

    backpack =
        OpenEngine::getInstance().getServerWorldController()->createBackpack(
            info.backpackCapacity);

    m_healthPercent = std::make_shared<float>(1.0f);
    m_movable = eInfo.movable;

    pProperties.setTileSize(static_cast<uint8_t>(info.widthFactor),
                            static_cast<uint8_t>(info.heightFactor));
    // 碰撞箱只取 Y 轴底部 2/3（向下取整），上半部分不参与碰撞
    pProperties.setCollisionHeightScale(2.0f / 3.0f);

    // 构建动画组映射表
    m_animMap.clear();
    for (const auto &group : eInfo.animations)
        m_animMap[group.id] = group;

    status = EntityStatus::Ready;
}

void Entity::createRenderer()
{
    if (!renderer && info.EntityTypeID != 0)
    {
        renderer = std::make_unique<Mob>(info.texture.meta.textureID,
                                         info.texture.meta.cols,
                                         info.texture.meta.rows);
        renderer->Configure().Anchor(AnchorPoint::BottomCenter).Alpha(0.0f);

        widthRelative = 1.0f / *OpenCoreManagers::SetManager.getRenderWidth();
        heightRelative = 1.0f / *OpenCoreManagers::SetManager.getRenderHeight();

        renderer->setTileSize(pProperties.getTileWidth(),
                              pProperties.getTileHeight());
        renderer->Configure().Scale(info.widthFactor * widthRelative, 0.0f);

        m_baseFrame = static_cast<uint16_t>(info.texture.originRow *
                                                info.texture.meta.cols +
                                            info.texture.originCol);
    }
}

void Entity::Draw(const Vec3 &absPos)
{
    if (!drawable)
        return;

    if (!renderer)
        createRenderer();

    renderer->setPosition(absPos.x, absPos.y);
    renderer->setTransparency(1.0f);
    renderer->getVisualState()->frameIndex = getCurrentFrameIndex();
    renderer->Draw();
}

void Entity::Draw(float cameraX, float cameraY)
{
    if (drawable)
    {
        if (!renderer)
            createRenderer();

        Vec3 Position = pProperties.getPosition();

        auto renderWidth = *OpenCoreManagers::SetManager.getRenderWidth();
        auto renderHeight = *OpenCoreManagers::SetManager.getRenderHeight();

        renderWidth = (renderWidth - 2) / 2 + 1;
        renderHeight = (renderHeight - 1) / 2 + 1;

        if ((abs(cameraX - Position.x) > renderWidth) or
            (cameraY - Position.y > renderHeight))
            return;

        renderer->setPosition(0.5f + (Position.x - cameraX) * widthRelative,
                              0.5f + (Position.y - cameraY) * heightRelative);
        renderer->setTransparency(1.0f);
        renderer->getVisualState()->frameIndex = getCurrentFrameIndex();
        renderer->Draw();
    }
    else if (renderer)
    {
        renderer.reset();
    }
}

void Entity::onUpdate(float totalTime)
{
    if (lastTime < 0.0f)
    {
        lastTime = totalTime;
        return;
    }
    float deltaTime = totalTime - lastTime;
    Vec3 previousPosition = pProperties.getPosition();

    // 刷新
    pProperties.onUpdate(totalTime);

    if (!m_movable)
    {
        pProperties.setSpeed({0, 0, 0});
        pProperties.setDesiredVelocity({0, 0, 0});
    }
    else
    {
        // 碰撞检测：若新位置不可通行则回退
        Vec3 newPosition = pProperties.getPosition();
        if (!canMoveTo(newPosition))
        {
            pProperties.setPosition(previousPosition);
            pProperties.setSpeed({0, 0, 0});
        }

        // ─── 动画状态机 ───────────────────────────────────
        Vec3 spd = pProperties.getSpeed();
        bool moving = (spd.x != 0.0f || spd.y != 0.0f);

        if (moving)
        {
            short animId = directionToAnimId(pProperties.getDirection());

            if (animId != m_currentAnimId)
            {
                m_currentAnimId = animId;
                m_currentFrame = 0;
                m_frameTimer = 0.0f;
            }

            auto it = m_animMap.find(m_currentAnimId);
            if (it != m_animMap.end() && !it->second.frames.empty())
            {
                auto &group = it->second;
                m_frameTimer += deltaTime;
                float frameInterval = 1.0f / group.frameRate;
                while (m_frameTimer >= frameInterval)
                {
                    m_frameTimer -= frameInterval;
                    m_currentFrame = (m_currentFrame + 1) % group.frames.size();
                }
            }
        }
        else
        {
            m_currentAnimId = 0;
            m_currentFrame = 0;
            m_frameTimer = 0.0f;
        }
    }

    // 刷新血量
    *m_healthPercent -= 0.05f * deltaTime;

    if (*m_healthPercent <= 0.0f)
        *m_healthPercent = 1.0f;

    lastTime = totalTime;
}

short Entity::directionToAnimId(Direction dir)
{
    switch (dir)
    {
    case Direction::Up:
        return 1;
    case Direction::Down:
        return 2;
    case Direction::Left:
        return 3;
    case Direction::Right:
        return 4;
    default:
        return 0;
    }
}

uint16_t Entity::getCurrentFrameIndex() const
{
    if (m_currentAnimId != 0)
    {
        auto it = m_animMap.find(m_currentAnimId);
        if (it != m_animMap.end() && m_currentFrame < it->second.frames.size())
        {
            const auto &frame = it->second.frames[m_currentFrame];
            return static_cast<uint16_t>(
                frame.originRow * info.texture.meta.cols + frame.originCol);
        }
    }
    return m_baseFrame;
}

bool Entity::canMoveTo(const Vec3 &pos) const
{
    auto wc = OpenEngine::getInstance().getServerWorldController();
    if (!wc)
        return true;

    uint8_t tw = pProperties.getTileWidth();
    uint8_t effH = pProperties.getEffectiveTileHeight();
    uint8_t th = pProperties.getTileHeight();

    Vec3 spd = pProperties.getSpeed();
    int halfW = tw / 2;
    int baseX =
        static_cast<int>(std::floor(pos.x + (spd.x > 0.0f ? 1.0f : 0.0f))) -
        halfW;
    int baseY =
        static_cast<int>(std::floor(pos.y + (spd.y > 0.0f ? 1.0f : 0.0f)));

    for (uint8_t dy = 0; dy < effH; ++dy)
        for (uint8_t dx = 0; dx < tw; ++dx)
        {
            int cx = baseX + dx;
            int cy = baseY - dy;
            auto block = wc->queryBlockInfo(cx, cy);
            if (block.has_value() && block->Access == 0)
            {
                // LOG("[canMoveTo] BLOCKED at ({},{}) for entity at "
                //     "pos({:.2f},{:.2f}) tw={} effH={} th={} halfW={} baseX={}
                //     " "baseY={}", cx, cy, pos.x, pos.y, tw, effH, th, halfW,
                //     baseX, baseY);
                return false;
            }
        }

    return true;
}
