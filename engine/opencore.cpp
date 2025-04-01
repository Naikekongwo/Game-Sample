#include "opencore.h"
#include "Timer.h"

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

    stageController sController(graphic->getRenderer(), graphic->getWindow(), &timer);

    sController.pushStage(new game_preload(&sController)); // 添加游戏预加载场景

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
                sController.handleEvent(event); // 处理事件
                break;
            }

            SDL_Delay(1000/FRAME_RATE);

            SDL_RenderClear(graphic->getRenderer());

            timer.onUpdate();
            sController.onUpdate(); // 更新场景

            sController.onRender(); // 渲染场景

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