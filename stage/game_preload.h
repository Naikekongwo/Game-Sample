#ifndef _GAME_PRELOAD_H_
#define _GAME_PRELOAD_H_

#include "stage_controller.h"

class game_preload : public stage
{
    private:
    stage_controller* scontroller;
    Timer* time;
    std::vector<SDL_Texture*> assets;
    
    public:
    game_preload(stage_controller* controller, Timer* time);

    void handleEvents(SDL_Event &event) override;
    void onUpdate() override;
    void onDraw(SDL_Renderer* renderer) override;
};

#endif //_GAME_PRELOAD_H_