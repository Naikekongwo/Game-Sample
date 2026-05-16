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

    pProperties.setTileSize(static_cast<uint8_t>(info.widthFactor),
                            static_cast<uint8_t>(info.heightFactor));

    status = EntityStatus::Ready;
}

void Entity::createRenderer()
{
    if (!renderer && info.EntityTypeID != 0)
    {
        renderer =
            std::make_unique<Mob>(info.texture.textureID, info.texture.gridCols,
                                  info.texture.gridRows);
        renderer->Configure().Anchor(AnchorPoint::BottomCenter).Alpha(0.0f);

        widthRelative = 1.0f / *OpenCoreManagers::SetManager.getRenderWidth();
        heightRelative = 1.0f / *OpenCoreManagers::SetManager.getRenderHeight();

        renderer->setTileSize(pProperties.getTileWidth(),
                              pProperties.getTileHeight());
        renderer->Configure().Scale(info.widthFactor * widthRelative, 0.0f);

        m_baseFrame = static_cast<uint16_t>(info.texture.originRow *
                                                info.texture.gridCols +
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
    renderer->getVisualState()->frameIndex = m_baseFrame;
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
        renderer->getVisualState()->frameIndex = m_baseFrame;
        renderer->Draw();
    }
    else if (renderer)
    {
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

    // 刷新血量
    *m_healthPercent -= 0.05f * deltaTime;

    if (*m_healthPercent <= 0.0f)
        *m_healthPercent = 1.0f;

    lastTime = totalTime;
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
            if (block.has_value() && block->Access == 0)
                return false;
        }

    return true;
}
