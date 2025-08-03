// OpenCore.cpp
// OpenCore 的具体实现

#include <iostream>
#include <memory>

#include "OpenCore/OpenCore.h"


// 单例
OpenEngine& OpenEngine::getInstance()
{
    static OpenEngine instance;
    return instance;
}

bool OpenEngine::Run()
{
    // 初始化函数
    if(!Initialize()) { return false; }

    // 主循环
    if(!MainLoop()) { return false; }

    // 生命周期结束
    if(!CleanUp()) { return false; }

    return true;
}


bool OpenEngine::Initialize()
{
    // 创建 GFX 实例
    gfxInstance = &GraphicsManager::getInstance();

    
    // 创建资源管理器实例
    resManager = &ResourceManager::getInstance();

    
    // 创建音效管理器实例
    sfxManager = &SoundEffectManager::getInstance();
    // 创建场景控制器实例
    sController = std::make_unique<StageController>();
    // 创建计时器实例
    timer = std::make_unique<Timer>(30);

    // 初始化 GFX 实例 (若失败直接退出)
    if(!gfxInstance->Init()) return false;
    // 初始化资源管理器
    resManager->Init(gfxInstance->getRenderer());
    // 初始化音效管理器
    sfxManager->Init(resManager);

    return true;
    // 初始化成功
}

bool OpenEngine::MainLoop()
{
    bool should_close = false;
    SDL_Event event;
    
    sController->changeStage(std::make_unique<PreloadStage>(gfxInstance->getRenderer(), resManager, sfxManager, timer.get()));

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
        // SDL_Log("Delta Time: %f, Delay Time: %f", timer->getDeltaTime(), timer->getDelayTime()*1000);
    }

    return true;
}


bool OpenEngine::CleanUp()
{
    sController.reset();
    timer.reset();

    sfxManager->CleanUp();
    resManager->CleanUp();

    gfxInstance->CleanUp();
    
    return true;
}