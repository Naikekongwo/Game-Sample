#include "OpenCore/OpenCore.hpp"
#include <SDL2/SDL_keycode.h>
#include <memory>

MapExplorer::MapExplorer(const string &id, short layer)
    : UIElement(id, layer, nullptr)
{
    // 初始化函数
}

void MapExplorer::onEnter()
{
    // 正式开始渲染准备
    if (status == MapExpStatus::Creating)
    {
        // 加载地图
        map = std::make_unique<ClassicMap>(1, mapPath);
        map->onEnter();

        // 渲染 worker 初始化
        renderWorker = UI<ImageBoard>("worker", 0, 2035, 16, 16);
        renderWorker->Configure()
            .Parent(this)
            .Anchor(AnchorPoint::Center)
            .Alpha(0.0f);
        // 设置缩放，Y=0 表示保持比例
        uint8_t renderWidth = *OpenCoreManagers::SetManager.getRenderWidth();
        uint8_t renderHeight = *OpenCoreManagers::SetManager.getRenderHeight();

        widthFactor = 1.0f / renderWidth;
        heightFactor = 1.0f / renderHeight;

        renderWorker->setScale(widthFactor, 0.0f);

        left_border = (renderWidth - 2) / 2 + 1;
        up_border = (renderHeight - 1) / 2 + 1;

        Vec2 pos{24, 24};
        pProperties.setPosition(pos);

        status = MapExpStatus::Ready;
    }
}

void MapExplorer::onRender()
{
    if (status != MapExpStatus::Ready)
        return;

    Vec2 Position = pProperties.getPosition();

    // 取整数格子索引
    int center_x = Position.x + 0.5f;
    int center_y = Position.y + 0.5f;

    float offsetX = Position.x - center_x;
    float offsetY = Position.y - center_y;

    for (int y = -up_border; y <= up_border; ++y)
    {
        for (int x = -left_border; x <= left_border; ++x)
        {
            int gx = x + center_x;
            int gy = y + center_y;

            BlockInfo &info = map->getBlockInfo(gx, gy);

            // 渲染时加入 sub-grid 偏移
            renderWorker->setPosition((x - offsetX) * widthFactor + 0.5f,
                                      (y - offsetY) * heightFactor + 0.5f);

            renderWorker->setTransparency(1.0f);
            renderWorker->getAnimationState()->frameIndex = info.Terrain;
            renderWorker->onRender();
        }
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
        pProperties.onUpdate(totalTime);
    }
}

void MapExplorer::handlEvents(SDL_Event &event, float totalTime)
{
    if (status == MapExpStatus::Ready)
    {
        if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_w:
            {
                Vec2 speed{0, 5};
                pProperties.giveSpeed(speed);
                break;
            }
            case SDLK_s:
            {
                Vec2 speed{0, -5};
                pProperties.giveSpeed(speed);
                break;
            }
            case SDLK_a:
            {
                Vec2 speed{5, 0};
                pProperties.giveSpeed(speed);
                break;
            }
            case SDLK_d:
            {
                Vec2 speed{-5, 0};
                pProperties.giveSpeed(speed);
                break;
            }
            default:
                break;
            }
        }
    }
}