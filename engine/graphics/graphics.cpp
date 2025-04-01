#include "graphics.h"
#include "../../macro.h"

#include <iostream>

bool graphics::Init()
{
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) return false;

    if(IMG_Init(IMG_INIT_PNG) < 0) return false;

    if(SDL_CreateWindowAndRenderer(1280,720,SDL_RENDERER_ACCELERATED,&window,&renderer) != 0) return false;

    // SDL_Surface* iconSurface = SDL_LoadBMP(ICON_PATH);
    SDL_Surface* iconSurface = IMG_Load(ICON_PATH);
    if(!iconSurface) return false;

    SDL_SetWindowIcon(window, iconSurface);
    SDL_SetWindowTitle(window,GAME_TITLE);
    SDL_SetWindowResizable(window,SDL_TRUE);
    SDL_SetWindowMinimumSize(window,1280,720);

    int width, height;
    SDL_GetRendererOutputSize(renderer, &width, &height);
    
    std::cout<<width<<height<<std::endl;

    return true;
}