#ifndef _GFXCORE_H_
#define _GFXCORE_H_

// GfxCore.h
// 图形核心

#include <string>
#include <SDL2/SDL.h>
#include <STB-IMAGE/stb_image.h>

SDL_Surface* stbi_loadSurface(const char* filename);// STB库加载图片到 SDL_Surface 【不推荐使用，性能差】
    void FreeStbiSurface(SDL_Surface* surface);// 销毁STB库加载的Surface

namespace Algorithms {
    SDL_Texture* STB_SDL_LOAD(const std::string &path, SDL_Renderer* renderer);// 多功能加载函数
};


class GraphicsInstance
{
    public:
    ~GraphicsInstance();
    
    bool Init();

    SDL_Window *getWindow() const { return window; }
    SDL_Renderer *getRenderer() const { return renderer; }

    private:
    SDL_Window *window;
    SDL_Renderer *renderer;
};

#endif //_GFXCORE_H_