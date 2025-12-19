#include "OpenCore/OpenCore.hpp"
#include "Eclipsea/Eclipsea.hpp"
#include <SDL2/SDL_render.h>


FrameCounter::FrameCounter(const std::string& id, uint8_t layer, Texture *texture)
{
    this->id = id;
    this->layer = layer;

    this->directRender = false;

    this->AnimeState = std::make_unique<AnimationState>();

    this->AnimeManager = std::make_unique<AnimationManager>();

}

void FrameCounter::handlEvents(SDL_Event &event, float totalTime)
{
    
}

void FrameCounter::onRender()
{
    auto &Res = ResourceManager::getInstance();
    TTF_Font* font = Res.GetFont(fontID);
    if(!font) return;

    std::string fpsText = "FPS: " + std::to_string(static_cast<int>(FPS));
    SDL_Color color = {255, 255, 255, 255};

    SDL_Surface* textSurface = TTF_RenderText_Blended(font, fpsText.c_str(), color);
    if(!textSurface) return;

    auto &GFX = GraphicsManager::getInstance();
    auto GF = GFX.getRenderEngine();

    auto SDLGF = static_cast<SDL_Adapter*>(GF);

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(SDLGF->getRenderer(), textSurface);
    if(!textTexture) {
        SDL_FreeSurface(textSurface);
        return;
    }

    OpenCore_Rect rect {0, 0, textSurface->w, textSurface->h};

    auto entry = GFX.getRenderEngine();

    auto si = static_cast<SDL_Adapter*>(entry);

    SDL_Rect Srect {rect.x, rect.y, rect.w, rect.h};

    SDL_RenderCopyEx(si->getRenderer(),textTexture, nullptr, &Srect, 0.0, nullptr, SDL_FLIP_NONE);

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
}


void FrameCounter::onUpdate(float totalTime)
{
    if(totalTime > lastTime)
    {
        FPS = 1.0f / (totalTime - lastTime);
        lastTime = totalTime;   
    }
    
    AnimeManager->onUpdate(totalTime, *AnimeState.get());
}