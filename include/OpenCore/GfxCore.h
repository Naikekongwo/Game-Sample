#ifndef _GFXCORE_H_
#define _GFXCORE_H_

// GfxCore.h
// 图形核心

#include "Macros.h"
#include <SDL2/SDL.h>


#include <STB-IMAGE/stb_image.h>
#define STB_IMAGE_IMPLEMENTATION


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