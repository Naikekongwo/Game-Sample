// GfxCoe.cpp
// 负责 GFX_CORE 的方法实现

#include "OpenCore/OpenCore.hpp"
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <cstddef>

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
        Console_Log("TTF_Init Error: %s", TTF_GetError());
        return -1;
    }

    // 其他平台的初始化方法
    if (SDL_CreateWindowAndRenderer(1280, 720, SDL_RENDERER_ACCELERATED,
                                    &window, &renderer) != 0)
    {
        return false;
    }

    SDL_SetWindowTitle(window, "OpenCore Window");

    SDL_SetWindowResizable(window, SDL_TRUE);

    // 初始化成功
    return true;
}

void GraphicsManager::refreshWindowProperties()
{
    auto gameInfo = OpenEngine::getInstance().getGameInfo();

    if (!gameInfo)
    {
        Console_Log(
            "GraphicsManager::Window Encountered a null pointer for game "
            "info!");
        return;
    }

    std::string title = OpenEngine::getInstance().getGameInfo()->gameName;

    if (gameInfo->beta == true)
    {
        title = title + " : " +
                std::to_string(static_cast<int>(gameInfo->version_major)) +
                "." +
                std::to_string(static_cast<int>(gameInfo->version_minor)) +
                std::string(" Beta with OpenCore ") +
                std::to_string(static_cast<int>(ENGINE_VERSION_MAJOR)) + "." +
                std::to_string(static_cast<int>(ENGINE_VERSION_MINOR));
    }

    SDL_SetWindowTitle(window, title.c_str());

    // 预计在 OpenCore 27.1解决该问题
    if (gameInfo->nearestScaling)
    {
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    }

    OpenCoreManagers::SetManager.setTargetWidth(
        gameInfo->TargetResolutionWidth, gameInfo->TargetResolutionHeight);

    SDL_RenderSetLogicalSize(renderer, gameInfo->TargetResolutionWidth,
                             gameInfo->TargetResolutionHeight);

    TargetWindowWidth = gameInfo->TargetResolutionWidth;
    TargetWindowHeight = gameInfo->TargetResolutionHeight;
}

void GraphicsManager::CleanUp()
{
    if (!renderer)
        Console_Log("GFXManager::CleanUp() failed to destroy a null renderer.");
    else
        SDL_DestroyRenderer(renderer);

    if (!window)
        Console_Log("GFXManager::CleanUp() failed to destroy a null window.");
    else
        SDL_DestroyWindow(window);

    Console_Log("GFXManager::CleanUp() has reseted the scale system.");

    Console_Log("GFXManager::CleanUp() manage to quit the SDL.");
    SDL_Quit();
}

int GraphicsManager::Draw(SDL_Texture *texture, const Rect *srcRect,
                          const Rect *dstRect, const double angle,
                          const Point *center)
{
    SDL_Rect source;
    SDL_Rect destination;
    SDL_Point point;

    if (!texture)
    {
        SDL_Log("GFXManager::Draw Encountered a empty texture.");
        return -1;
    }
    if (srcRect)
    {
        source = *srcRect;
    }
    if (dstRect)
    {
        destination = *dstRect;
    }
    if (center)
    {
        point = *center;
    }

    return SDL_RenderCopyEx(renderer, texture, (srcRect) ? &source : nullptr,
                            (dstRect) ? &destination : nullptr, angle,
                            (center) ? &point : nullptr, SDL_FLIP_NONE);
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
        Console_Log("Failed to set render target: %s", SDL_GetError());
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
