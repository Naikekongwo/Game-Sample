// OpenCore.cpp
// OpenCore 的具体实现

#include <iostream>
#include <memory>

#include "OpenCore/OpenCore.h"


bool OpenEngine::Run()
{
    gfxInstance = new GFXinstance();

    // 初始化图形核心，若失败则退出
    if(!gfxInstance->Init()) { return false; }
    // 初始化资源管理器
    resManager = &ResourceManager::Get();
    // 初始化音效管理器
    sfxManager = new SoundEffectManager(resManager);
    // 初始化场景管理器
    sController = new StageController();
    // 初始化时间管理器
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
    
    sController->changeStage(std::make_unique<PreloadStage>(gfxInstance->getRenderer(), resManager, sfxManager));

    while(!should_close)
    {
        // 事件处理
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT) 
                should_close = true;
            else 
                sController->handlEvents(&event);
        }

        timer->Tick();

        SDL_Delay(timer->getDelayTime()); // 限制帧率，避免CPU飙高

        sController->onUpdate();
        sController->onRender();

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