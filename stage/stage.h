#ifndef _STAGE_H_
#define _STAGE_H_

#include <SDL2/SDL.h>

class stage
{
    public:
    virtual ~stage() {};
    
    virtual void handleEvents(SDL_Event &event) = 0;
    virtual void onUpdate() = 0;
    virtual void onDraw(SDL_Renderer* renderer) = 0;

};

#endif //_STAGE_H_