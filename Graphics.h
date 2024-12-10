#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <SDL2/SDL.h>

class SDL_Adapter {
    public:
    bool run();
    bool Init();
    bool Load();
    void CleanUp();

    private:
    SDL_Window* window;
    SDL_Surface* winSurface;
    SDL_Surface* studioLOGO;
};


#endif //_GRAPHICS_H_