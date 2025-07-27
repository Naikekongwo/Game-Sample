#ifndef _GFXCORE_H_
#define _GFXCORE_H_

// GfxCore.h
// 图形核心


#include <STB-IMAGE/stb_image.h>

#include "Macros.h"
#include <SDL2/SDL.h>

// STB库加载图片到 SDL_Surface 【不推荐使用，性能差】
SDL_Surface* stbi_loadSurface(const char* filename);

// 销毁STB库加载的Surface
void FreeStbiSurface(SDL_Surface* surface);

class GFXinstance
{
    public:
    ~GFXinstance();
    
    bool Init();

    SDL_Window *getWindow() const { return window; }
    SDL_Renderer *getRenderer() const { return renderer; }

    private:
    SDL_Window *window;
    SDL_Renderer *renderer;
};

#endif //_GFXCORE_H_