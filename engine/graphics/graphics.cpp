#include "graphics.h"
#include "../../macro.h"

#include <iostream>

bool graphics::Init()
{
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) return false;
    if(SDL_CreateWindowAndRenderer(WD_WIDTH,WD_HEIGHT,SDL_RENDERER_ACCELERATED,&window,&renderer) != 0) return false;

    SDL_Surface* iconSurface = SDL_LoadBMP(ICON_PATH);
    if(!iconSurface) return false;

    SDL_SetWindowIcon(window, iconSurface);
    SDL_SetWindowTitle(window,GAME_TITLE);

    int width, height;
    SDL_GetRendererOutputSize(renderer, &width, &height);
    
    std::cout<<width<<height<<std::endl;

    return true;
}