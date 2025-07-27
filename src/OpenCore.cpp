#include "OpenCore/OpenCore.h"


bool OpenEngine::Run()
{
    gfxInstance = new GFXinstance();

    // 初始化图形核心，若失败则退出
    if(!gfxInstance->Init()) { return false; }

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
    
    while(!should_close)
    {
        // 事件处理
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                should_close = true;
            }
        }

        SDL_SetRenderDrawColor(gfxInstance->getRenderer(), 0, 0, 0, 255);
        SDL_RenderClear(gfxInstance->getRenderer());
        SDL_RenderPresent(gfxInstance->getRenderer());

        SDL_Delay(16); // 限制帧率，避免CPU飙高

        // 渲染逻辑（如果有的话）
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
}