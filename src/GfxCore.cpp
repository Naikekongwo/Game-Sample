// 负责 GFX_CORE 的方法实现

#include "OpenCore/GfxCore.h"


GFXinstance::~GFXinstance() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

bool GFXinstance::Init() {
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) { return false;}
    // 初始化 SDL 失败返回 false

    if(SDL_CreateWindowAndRenderer(1280, 720, 0 , &window, &renderer) != 0) {return false;}
    // 初始化窗口和渲染器则返回 false


    return true;
};