// OpenCore.cpp
// OpenCore 的具体实现

#include <iostream>
#include <memory>

#include "OpenCore/OpenCore.h"


bool OpenEngine::Run()
{
    // 初始化函数
    if(!Initialize()) { return false; }

    // 主循环
    if(!MainLoop()) { return false; }

    // 生命周期结束
    return true;
}


bool OpenEngine::Initialize()
{
    // 创建 GFX 实例
    gfxInstance = std::make_unique<GFXinstance>();

    // 初始化 GFX 实例 (若失败直接退出)
    if(!gfxInstance->Init()) return false;

    // 创建资源管理器实例
    resManager = &ResourceManager::Get();

    // 初始化资源管理器
    resManager->SetRenderer(gfxInstance->getRenderer());

    // 创建音效管理器实例
    sfxManager = std::make_unique<SoundEffectManager>(resManager);

    // 创建场景控制器实例
    sController = std::make_unique<StageController>();

    // 创建计时器实例
    timer = std::make_unique<Timer>(30);

    return true;
    // 初始化成功
}

bool OpenEngine::MainLoop()
{
    bool should_close = false;
    SDL_Event event;
    
    sController->changeStage(std::make_unique<PreloadStage>(gfxInstance->getRenderer(), resManager, sfxManager.get(), timer.get()));

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
        SDL_RenderClear(gfxInstance->getRenderer());
        sController->onRender();

        SDL_RenderPresent(gfxInstance->getRenderer());
        SDL_Log("Delta Time: %f, Delay Time: %f", timer->getDeltaTime(), timer->getDelayTime()*1000);
    }

    return true;
}
