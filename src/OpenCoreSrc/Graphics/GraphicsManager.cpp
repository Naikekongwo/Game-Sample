// GfxCoe.cpp
// 负责 GFX_CORE 的方法实现

#include "OpenCore/OpenCore.hpp"
#include "Eclipsea/Eclipsea.hpp"

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

    // PSP 平台的 window 和 renderer 初始化方法
    // window = SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 480, 272, SDL_WINDOW_SHOWN);
    // renderer = SDL_CreateRenderer(window, -1 , SDL_RENDERER_ACCELERATED);

    // 将缩放的品质调为0，这样可以获得最锐利的缩放
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    
    // 其他平台的初始化方法
    if (SDL_CreateWindowAndRenderer(BASE_WINDOW_WIDTH, BASE_WINDOW_HEIGHT, SDL_RENDERER_ACCELERATED, &window, &renderer) != 0)
    {
        return false;
    }

    std::string title = APP_NAME;

    if(BOOL_IS_BETA == true)
    {
        title = APP_NAME + std::string(" Beta with OpenCore ") +
        std::to_string(static_cast<int>(ENGINE_VERSION_MAJOR)) + "." +
        std::to_string(static_cast<int>(ENGINE_VERSION_MINOR));
    }

    SDL_SetWindowTitle(window, title.c_str());
    SDL_SetWindowResizable(window, SDL_TRUE);

    ContentScale = std::make_unique<ScaleManager>(BASE_WINDOW_WIDTH, BASE_WINDOW_HEIGHT);
    // ContentScale->UpdateTargetSize(480, 272);

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

void GraphicsManager::setScale(uint16_t w, uint16_t h)
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

int GraphicsManager::RenderCopy(SDL_Texture *texture,
                                  const SDL_Rect *srcrect,
                                  const SDL_Rect *dstrect,
                                  const double angle,
                                  const SDL_Point *center,
                                  const SDL_RendererFlip flip)
{
    return SDL_RenderCopyEx(renderer,
                            texture,
                            srcrect,
                            dstrect,
                            angle,
                            center,
                            flip);
}


SDL_Texture* GraphicsManager::createTexture(uint16_t w, uint16_t h)
{
    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        w, h
    );
    // 启用透明混合
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    return texture;
}

int GraphicsManager::setOffScreenRender(SDL_Texture* texture)
{
    if(SDL_SetRenderTarget(renderer, texture) != 0) {
        SDL_Log("Failed to set render target: %s", SDL_GetError());
        return -1;
    }

    if(texture) {
        // 可选：只在需要清理时才清理
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
    }

    return 0;
}
