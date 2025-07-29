#include "OpenCore/OpenCore.h"
#include "Stage/StageController.h"

bool OpenEngine::Run()
{
    gfxInstance = new GFXinstance();

    // 初始化图形核心，若失败则退出
    if(!gfxInstance->Init()) { return false; }


    timer = new Timer();

    // 主循环
    if(!MainLoop()) { return false; }

    // 回收资源
    CleanUp();

    // 生命周期结束
    return true;
}

bool OpenEngine::MainLoop()
{
    bool should_close = false;
    SDL_Event event;

    StageController sCon;

    PreloadStage pStage(gfxInstance->getRenderer());
    sCon.changeStage(&pStage);

    
    while(!should_close)
    {
        // 事件处理
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                should_close = true;
            }
            else
            {
                sCon.handlEvents(&event);
            }
        }

        timer->Tick();

        SDL_Delay(timer->getDelayTime()); // 限制帧率，避免CPU飙高

        sCon.onUpdate();
        sCon.onRender();

        SDL_Log("Delta Time: %f, Delay Time: %f", timer->getDeltaTime(), timer->getDelayTime()*1000);
    }

    return true;
}

void OpenEngine::CleanUp()
{
    if(gfxInstance)
    {
        delete gfxInstance;
        gfxInstance = nullptr;
    }

    if(timer)
    {
        delete timer;
        timer = nullptr;
    }
}