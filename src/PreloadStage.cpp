// PreloadStage.cpp
// 预加载页面的实现

#include "OpenCore/OpenCore.h"

PreloadStage::PreloadStage(SDL_Renderer* render, ResourceManager *resMana, SoundEffectManager *sfxMana, Timer* timer)
{
    renderer = render;
    resourceManager = resMana;
    sfxManager = sfxMana;
    this->timer = timer;

    LoadResources();
}

void PreloadStage::LoadResources()
{
    resourceManager->LoadMusicAsync(0, TEST_MUSIC); // 加载背景音乐

    std::vector<std::pair<short, std::string>> assets = {
        {0, RES_GAME_ICON},
        {1, HEADS_ICON}
    };

    for (const auto& [id, path] : assets) {
        TextureLoadTask task;
        task.id = id;
        task.path = path;
        task.future = resourceManager->LoadTextureAsync(id, path);
        textureTasks.push_back(std::move(task));
    }
    SDL_Log("PreloadStage: Resources loading started.");
}

bool PreloadStage::handlEvents(SDL_Event* event)
{
    return true;
    // 默认成功
}

void PreloadStage::onUpdate()
{
    bool allReady = true;
    for (auto& task : textureTasks) {
        if (!task.isFinished) {
            if (task.future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
                try {
                    task.future.get();
                } catch (const std::exception& e) {
                    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Exception in async load: %s", e.what());
                }
                task.isFinished = true;
            } else {
                allReady = false;
            }
        }
    }

    if (allReady && stageState == 0) {
        stageState = 1;

        sfxManager->loadBGMR(0); // BGM
        sfxManager->playBGM();
        sfxManager->setVolume(64); // 加载背景音乐

        stageState = 2;

        SDL_Log("PreloadStage: All resources loaded successfully.");
        std::unique_ptr<Sprite> heads = std::make_unique<Sprite>(1);
        // 创建人头精灵
        heads->setRect(512, 232, 256, 256);
        // 设置渲染区域
        heads->setTextureScale(256,256);
        // 设置单帧尺寸

        std::shared_ptr<FrameAnimation> headAnim = std::make_shared<FrameAnimation>(3, 3, true);
        // 创建帧动画
        
        heads->setAnimation(headAnim);
        
        heads->resetAnime(timer->getTotalTime());

        sprites.push_back(std::move(heads));

    }

    if (stageState == 2)
    {
        // 更新状态
       for(auto &sprite: sprites)
       {
        //遍历精灵表
        sprite->onUpdate(timer->getTotalTime());
       }
    }
}

void PreloadStage::onRender()
{
    if (stageState == 2)
    {
        // 更新状态
       for(auto &sprite: sprites)
       {
        //遍历精灵表
        sprite->onRender(renderer);
       }
    }
}