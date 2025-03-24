#include "game_preload.h"

#include "../macro.h"
#include "../engine/graphics/graphics.h"
#include "../engine/graphics/modules.h"


game_preload::game_preload(stage_controller* controller, Timer* time)
{
    scontroller = controller;

    this->time = time;
    
    SDL_Surface* tempSurface;

    assets.pushTexture(LoadTextureFromBMP(scontroller->getRenderer(),GAME_PRELOAD_ENGINE_PATH));

    tempSurface = SDL_LoadBMP(GAME_PRELOAD_ENGINE_PATH);
    assets.push_back(SDL_CreateTextureFromSurface(scontroller->getRenderer(),tempSurface));

    tempSurface = SDL_LoadBMP(GAME_PRELOAD_ICON_PATH);
    assets.push_back(SDL_CreateTextureFromSurface(scontroller->getRenderer(),tempSurface));

    SDL_FreeSurface(tempSurface);
}


void game_preload::onUpdate()
{
    if(!assets.empty())
    {
        float alpha = (-5 * (time->getTime() * time->getTime()) + 300 * (time->getTime())) / 9;

        SDL_SetTextureAlphaMod(assets.back(),(alpha>=255)?255:alpha);


        int width,height;
        
        SDL_GetRendererOutputSize(scontroller->getRenderer(),&width,&height);

        SDL_Rect srcRect = {0,0,width,height};
        SDL_Rect dstRect = GetCenteredPos(500,500,srcRect);
        SDL_RenderCopy(scontroller->getRenderer(),assets.back(),NULL,&dstRect);
        if(time->getTime() == FRAME_RATE-1) assets.erase(assets.end()-1);
    }
}

void game_preload::handleEvents(SDL_Event &event)
{

}

void game_preload::onDraw(SDL_Renderer* renderer)
{

}