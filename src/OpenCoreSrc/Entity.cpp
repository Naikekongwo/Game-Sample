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

    status = EntityStatus::Ready;
}

void Entity::createRenderer()
{
    // 创建一个角色绘制器
    if (!renderer && info.EntityTypeID != 0)
    {
        renderer = std::make_unique<Mob>(info.TextureID);
        renderer->Configure().Anchor(AnchorPoint::BottomCenter).Alpha(0.0f);

        widthRelative = 1.0f / *OpenCoreManagers::SetManager.getRenderWidth();
        heightRelative = 1.0f / *OpenCoreManagers::SetManager.getRenderHeight();

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

    // 只负责动画方向（这是Entity自己的逻辑）
    switch (pProperties.getDirection())
    {
    case Direction::Up:
        renderer->getVisualState()->frameIndex = 12;
        break;
    case Direction::Down:
        renderer->getVisualState()->frameIndex = 0;
        break;
    case Direction::Left:
        renderer->getVisualState()->frameIndex = 4;
        break;
    case Direction::Right:
        renderer->getVisualState()->frameIndex = 8;
        break;
    default:
        break;
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
                                  0.5f + (Position.y - cameraY) * heightfactor);
            renderer->setTransparency(1.0f);
            switch (pProperties.getDirection())
            {
            case Direction::Up:
            {
                renderer->getVisualState()->frameIndex = 12;
                break;
            }
            case Direction::Down:
            {
                renderer->getVisualState()->frameIndex = 0;
                break;
            }
            case Direction::Left:
            {
                renderer->getVisualState()->frameIndex = 4;
                break;
            }
            case Direction::Right:
            {
                renderer->getVisualState()->frameIndex = 8;
                break;
            }
            default:
                break;
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
    // 刷新
    pProperties.onUpdate(totalTime);
}