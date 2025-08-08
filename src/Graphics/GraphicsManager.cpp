// GfxCoe.cpp
// 负责 GFX_CORE 的方法实现

#include "OpenCore/OpenCore.hpp"

GraphicsManager &GraphicsManager::getInstance()
{
    // 单例 ： 仅执行一次
    static GraphicsManager instance;
    // 这个instance的对象在应用的整个生命周期中只会执行一次。
    return instance;
}

bool GraphicsManager::Init()
{
    // 图形核心的初始化方法

    // 初始化SDL 失败即返回假
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        return false;
    }

    // PSP 应该如此创建
    // window = SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 480, 272, SDL_RENDERER_ACCELERATED);
    // renderer = SDL_GetRenderer(window);


    // 创建窗口和渲染器 失败即返回假
    // 此处可能影响“跨平台性能”
    if (SDL_CreateWindowAndRenderer(1920, 1080, SDL_RENDERER_ACCELERATED, &window, &renderer) != 0)
    {
        return false;
    }

    SDL_SetWindowTitle(window, APP_NAME);
    SDL_SetWindowResizable(window, SDL_TRUE);

    ContentScale = std::make_unique<ScaleManager>(BASE_WINDOW_WIDTH, BASE_WINDOW_HEIGHT);
    // 初始化成功
    return true;
}

void GraphicsManager::CleanUp()
{
    if (!renderer)
        SDL_Log("GFXManager::CleanUp() failed to destroy a null renderer.");
    else
        SDL_DestroyRenderer(renderer);

    if (!window)
        SDL_Log("GFXManager::CleanUp() failed to destroy a null window.");
    else
        SDL_DestroyWindow(window);

    ContentScale.reset();
    SDL_Log("GFXManager::CleanUp() has reseted the scale system.");

    SDL_Log("GFXManager::CleanUp() manage to quit the SDL.");
    SDL_Quit();
}

void GraphicsManager::setScale(int w, int h)
{
    ContentScale->UpdateTargetSize(w, h);
}

int GraphicsManager::RenderCopyEx(SDL_Texture *texture,
                                  const SDL_Rect *srcrect,
                                  const SDL_Rect *dstrect,
                                  const double angle,
                                  const SDL_Point *center,
                                  const SDL_RendererFlip flip)
{
    SDL_Rect newRect = ContentScale->ToScreen(*dstrect);
    return SDL_RenderCopyEx(renderer,
                            texture,
                            srcrect,
                            &newRect,
                            angle,
                            center,
                            flip);
}