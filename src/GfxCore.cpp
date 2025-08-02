// GfxCoe.cpp
// 负责 GFX_CORE 的方法实现

#include "OpenCore/OpenCore.h"


GraphicsInstance::~GraphicsInstance()
{
    // 图形核心的析构函数
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool GraphicsInstance::Init()
{
    // 图形核心的初始化方法

    // 初始化SDL 失败即返回假
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) { return false; }

    // 创建窗口和渲染器 失败即返回假
    if(SDL_CreateWindowAndRenderer(GFX_WINDOW_WIDTH,GFX_WINDOW_HEIGHT, SDL_RENDERER_ACCELERATED, &window, &renderer) != 0) { return false; }

    // 图标加载方法【NON STANDARD!!!临时使用】
    SDL_Surface *iconSurface = stbi_loadSurface(RES_GAME_ICON);
    if(!iconSurface) { return false;} // 图标加载失败推出

    // 窗口属性设置
    SDL_SetWindowIcon(window, iconSurface);
    SDL_SetWindowTitle(window, APP_NAME);

    // 释放资源
    FreeStbiSurface(iconSurface);

    // 初始化成功
    return true;
}