// GfxCoe.cpp
// 负责 GFX_CORE 的方法实现

#include "OpenCore/OpenCore.hpp"
#include "Eclipsea/Eclipsea.hpp"
#include <cstdio>
#include <memory>

GraphicsManager &GraphicsManager::getInstance()
{
    // 单例 ： 仅执行一次
    static GraphicsManager instance;
    // 这个instance的对象在应用的整个生命周期中只会执行一次。
    return instance;
}

bool GraphicsManager::Init()
{
    Renderer = std::make_unique<SDL_Adapter>();
    
    Window_Info wInfo {"Eclipsea 无水之境", false, {1280,720}};

    if(!Renderer->InitRenderer(wInfo))
    {
        std::printf("Failed to initialize the renderer!.");
        return false;
    }

    return true;
}

void GraphicsManager::CleanUp()
{
    if(Renderer) Renderer->RendererCleanUp();
}

void GraphicsManager::setScale(uint16_t w, uint16_t h)
{
    ContentScale->UpdateTargetSize(w, h);
}

IRenderer* GraphicsManager::getRenderEngine()
{
    return Renderer.get();
}

OpenCore_Tilemap* GraphicsManager::createTexture(uint16_t w, uint16_t h)
{
    return Renderer->createTexture(w, h);
}

int GraphicsManager::setOffScreenRender(OpenCore_Tilemap *tileMap)
{
    return Renderer->setOffScreenRenderer(tileMap);
}
