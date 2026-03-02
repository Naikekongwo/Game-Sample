// GfxCoe.cpp
// 负责 GFX_CORE 的方法实现

#include "OpenCore/OpenCore.hpp"
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_video.h>

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

    if (TTF_Init() == -1)
    {
        SDL_Log("TTF_Init Error: %s", TTF_GetError());
        return -1;
    }

    // 预计在 OpenCore 27.1解决该问题
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    // 其他平台的初始化方法
    if (SDL_CreateWindowAndRenderer(
            BASE_WINDOW_WIDTH * 0.5, BASE_WINDOW_HEIGHT * 0.5,
            SDL_RENDERER_ACCELERATED, &window, &renderer) != 0)
    {
        return false;
    }

    SDL_SetWindowTitle(window, "OpenCore Window");

    SDL_SetWindowResizable(window, SDL_TRUE);

    SDL_RenderSetLogicalSize(renderer, BASE_WINDOW_WIDTH, BASE_WINDOW_HEIGHT);

    // 初始化成功
    return true;
}

void GraphicsManager::refreshWindowProperties()
{
    auto gameInfo = OpenEngine::getInstance().getGameInfo();

    if (!gameInfo)
    {
        SDL_Log("GraphicsManager::Window Encountered a null pointer for game "
                "info!");
        return;
    }

    std::string title = OpenEngine::getInstance().getGameInfo()->gameName;

    if (BOOL_IS_BETA == true)
    {
        title = title + std::string(" Beta with OpenCore ") +
                std::to_string(static_cast<int>(ENGINE_VERSION_MAJOR)) + "." +
                std::to_string(static_cast<int>(ENGINE_VERSION_MINOR));
    }

    SDL_SetWindowTitle(window, title.c_str());
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

    SDL_Log("GFXManager::CleanUp() has reseted the scale system.");

    SDL_Log("GFXManager::CleanUp() manage to quit the SDL.");
    SDL_Quit();
}

int GraphicsManager::RenderCopyEx(SDL_Texture *texture, const SDL_Rect *srcrect,
                                  const SDL_Rect *dstrect, const double angle,
                                  const SDL_Point *center,
                                  const SDL_RendererFlip flip)
{
    return SDL_RenderCopyEx(renderer, texture, srcrect, dstrect, angle, center,
                            flip);
}

int GraphicsManager::RenderCopy(SDL_Texture *texture, const SDL_Rect *srcrect,
                                const SDL_Rect *dstrect, const double angle,
                                const SDL_Point *center,
                                const SDL_RendererFlip flip)
{
    return SDL_RenderCopyEx(renderer, texture, srcrect, dstrect, angle, center,
                            flip);
}

SDL_Texture *GraphicsManager::createTexture(uint16_t w, uint16_t h)
{
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                             SDL_TEXTUREACCESS_TARGET, w, h);
    // 启用透明混合
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    return texture;
}

int GraphicsManager::setOffScreenRender(SDL_Texture *texture)
{
    if (SDL_SetRenderTarget(renderer, texture) != 0)
    {
        SDL_Log("Failed to set render target: %s", SDL_GetError());
        return -1;
    }

    if (texture)
    {
        // 可选：只在需要清理时才清理
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
    }

    return 0;
}
