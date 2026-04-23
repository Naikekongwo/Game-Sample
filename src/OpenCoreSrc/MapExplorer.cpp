#include "OpenCore/OpenCore.hpp"
#include <SDL2/SDL_keycode.h>
#include <memory>

MapExplorer::MapExplorer(const string &id, short layer)
    : UIElement(id, layer, nullptr)
{

    OpenEngine::getInstance().getServerWorldController()->enabled(true);
}

void MapExplorer::onExit()
{
    OpenEngine::getInstance().getServerWorldController()->enabled(false);
}

void MapExplorer::onEnter()
{

    if (status == MapExpStatus::Creating)
    {
        status = MapExpStatus::Ready;
    }
}

void MapExplorer::Draw()
{
    if (status != MapExpStatus::Ready)
        return;

    auto World = OpenEngine::getInstance().getServerWorldController();
    World->Draw();
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
    auto World = OpenEngine::getInstance().getServerWorldController();

    if (status == MapExpStatus::Ready && World->isVisible())
    {
        auto &pProperties = World->getProperties();
        if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_w:
            {
                Vec3 speed{0, -5, 0};
                pProperties.addSpeed(speed);
                break;
            }
            case SDLK_s:
            {
                Vec3 speed{0, 5, 0};
                pProperties.addSpeed(speed);
                break;
            }
            case SDLK_a:
            {
                Vec3 speed{-5, 0, 0};
                pProperties.addSpeed(speed);
                break;
            }
            case SDLK_d:
            {
                Vec3 speed{5, 0, 0};
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