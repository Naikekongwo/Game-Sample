// OpenCore.cpp
// OpenCore 的具体实现

#include <iostream>
#include <memory>

#include "OpenCore/OpenCore.hpp"
#include "Union.hpp"

// 单例
OpenEngine &OpenEngine::getInstance()
{
    static OpenEngine instance;
    return instance;
}

bool OpenEngine::Run()
{
    // 初始化函数
    if (!Initialize())
    {
        return false;
    }

    // 主循环
    if (!MainLoop())
    {
        return false;
    }

    // 生命周期结束
    if (!CleanUp())
    {
        return false;
    }

    return true;
}

bool OpenEngine::Initialize()
{
    // 引用引擎所有管理类的命名空间
    using namespace OpenCoreManagers;

    // 创建 GFX 实例
    GFXManager;
    // 创建资源管理器实例
    ResManager;
    // 创建音效管理器实例
    SFXManager;
    // 创建场景控制器实例
    sController = std::make_unique<StageController>();
    // 创建计时器实例
    timer = std::make_unique<Timer>(30);

    // 初始化 GFX 实例 (若失败直接退出)
    if (!GFXManager.Init())
        return false;

    // 初始化资源管理器(其初始化时需要renderer，所以必须在GFX之后初始化)
    ResManager.Init();
    // 初始化音效管理器
    SFXManager.Init(&ResManager);

    return true;
    // 初始化成功
}

bool OpenEngine::MainLoop()
{
    using namespace OpenCoreManagers;

    bool should_close = false;
    SDL_Event event;

    sController->changeStage(std::make_unique<PreloadStage>(timer.get(), sController.get()));

    while (!should_close)
    {
        // 事件处理
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                should_close = true;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_F11)
                {
                    Uint32 flags = SDL_GetWindowFlags(GFXManager.getWindow());
                    if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
                    {
                        SDL_SetWindowFullscreen(GFXManager.getWindow(), 0);
                    }
                    else
                    {
                        SDL_SetWindowFullscreen(GFXManager.getWindow(), SDL_WINDOW_FULLSCREEN);
                    }
                }
                break;
            case SDL_WINDOWEVENT:
                switch (event.window.event)
                {
                    case SDL_WINDOWEVENT_RESIZED:
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                    {
                        GFXManager.setScale(event.window.data1, event.window.data2);
                        break;
                        // 注: data1 和 data2 分别是窗口的宽和高
                    }
                    default:
                        break;
                }
                break;
            default:
                break;
            }
            sController->handlEvents(&event);
        }

        timer->Tick();

        ResManager.ProcessMainThreadTasks();

        sController->onUpdate();

        SDL_Renderer *renderer = GFXManager.getRenderer();

        SDL_RenderClear(GFXManager.getRenderer());
        sController->onRender();

        SDL_RenderPresent(GFXManager.getRenderer());

        SDL_Delay(timer->getDelayTime());
        // 限制帧间隔

        // SDL_Log("Delta Time: %f, Delay Time: %f", timer->getDeltaTime(), timer->getDelayTime()*1000);
    }

    return true;
}

bool OpenEngine::CleanUp()
{
    using namespace OpenCoreManagers;

    sController.reset();
    timer.reset();

    SFXManager.CleanUp();
    ResManager.CleanUp();

    GFXManager.CleanUp();

    return true;
}