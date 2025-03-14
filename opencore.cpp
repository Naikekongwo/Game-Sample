#include "opencore.h"
#include "Timer.h"
#include "stage/game_preload.h"

bool openCore::Start()
{
    graphic = new graphics();
    if(!graphic->Init()) return false;

    MainLoop();

    CleanUp();

    return true;
}

bool openCore::MainLoop()
{
    bool should_close = false;

    SDL_Event event;

    Timer timer;

    stage_controller stageController;

    stageController.setRenderer(graphic->getRenderer());

    stage* prestage = new game_preload(&stageController,&timer);

    stageController.pushState(prestage);

    while(!should_close)
    {
        while(SDL_PollEvent(&event))
        {
            switch ((event.type))
            {
            case SDL_QUIT:
                should_close = true;
                break;
            default:
                stageController.handleEvents(event);
                break;
            }

            SDL_Delay(1000/15);

            SDL_RenderClear(graphic->getRenderer());

            stageController.onUpdate();
            timer.onUpdate();

            stageController.onDraw();
            SDL_RenderPresent(graphic->getRenderer());
        }
    }
    return true;
}

void openCore::CleanUp()
{
    graphic->~graphics();
    // 销毁SDL资源
}