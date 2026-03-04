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
        worldController.onEnter();
        status = MapExpStatus::Ready;
    }
}

void MapExplorer::onRender()
{
    if (status != MapExpStatus::Ready)
        return;

    worldController.Draw();
}

void MapExplorer::onUpdate(float totalTime)
{
    if (status == MapExpStatus::Creating)
    {
        onEnter();
    }
    else
    {
        worldController.onUpdate(totalTime);
    }
}

void MapExplorer::handlEvents(SDL_Event &event, float totalTime)
{
    if (status == MapExpStatus::Ready)
    {
        auto &pProperties = worldController.getProperties();
        if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_w:
            {
                Vec3 speed{0, 5, 0};
                pProperties.addSpeed(speed);
                break;
            }
            case SDLK_s:
            {
                Vec3 speed{0, -5, 0};
                pProperties.addSpeed(speed);
                break;
            }
            case SDLK_a:
            {
                Vec3 speed{5, 0, 0};
                pProperties.addSpeed(speed);
                break;
            }
            case SDLK_d:
            {
                Vec3 speed{-5, 0, 0};
                pProperties.addSpeed(speed);
                break;
            }
            case SDLK_SPACE:
            {
                Console_Log("Fuck");
                Vec3 speed{0, 0, 5};
                pProperties.addSpeed(speed);
                break;
            }
            default:
                break;
            }
        }
    }
}