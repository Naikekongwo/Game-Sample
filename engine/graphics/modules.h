#ifndef _MODULES_H_
#define _MODULES_H_

#include <SDL2/SDL.h>
#include <iostream>

#include "atlas.h"

SDL_Rect GetCenteredPos(int w, int h, SDL_Rect &srcRect)
{
    SDL_Rect temp = {srcRect.w/2-(w/2),srcRect.h/2-(h/2),w,h};
    return temp;
}

// 读入材质并转化为 TextureData
textureData* LoadTextureFromBMP(SDL_Renderer* renderer, std::string FilePath)
{
    SDL_Surface* temp = SDL_LoadBMP(FilePath.c_str());
    if(!temp) return nullptr;

    SDL_Texture* tTexture = SDL_CreateTextureFromSurface(renderer,temp);
    
    SDL_FreeSurface(temp);

    int width,height;

    SDL_QueryTexture(tTexture,NULL,NULL,&width,&height);

    textureData* newData = new textureData(tTexture,width,height);
    
    return newData ;
}


#endif //_MODULES_H_