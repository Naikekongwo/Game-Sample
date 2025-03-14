#ifndef _STAGE_CONTROLLER_H_
#define _STAGE_CONTROLLER_H_

#include <vector>
#include <SDL2/SDL.h>

#include "stage.h"
#include "../Timer.h"

// class stage;

class stage_controller
{
    private:
    std::vector<stage*> stages;
    SDL_Renderer* renderer;

    public:
    ~stage_controller()
    {
        for( auto stage: stages )
        {
            delete stage;
        }
    }

    void setRenderer(SDL_Renderer* renderer)
    {
        this->renderer = renderer;
    }

    SDL_Renderer* getRenderer()
    {
        return renderer;
    }

    void pushState(stage* stage)
    {
        stages.push_back(stage);
    }

    void handleEvents(SDL_Event &event)
    {
        if(! stages.empty()) stages.back()->handleEvents(event);
    }

    void onUpdate()
    {
        if(! stages.empty()) stages.back()->onUpdate();
    }

    void onDraw()
    {
        if(!stages.empty()) stages.back()->onDraw(renderer);
    }
};

#endif //_STAGE_CONTROLLER_H_