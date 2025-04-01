#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

// graphics.h 图形引擎

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "modules.h"


class graphics
{
public:
    ~graphics()
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    bool Init();

    SDL_Window *getWindow(){return this->window;}
    SDL_Renderer *getRenderer(){return this->renderer;}

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
};

#endif //_GRAPHICS_H_