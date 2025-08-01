// PreloadStage.cpp
// 预加载页面的实现

#include "OpenCore/OpenCore.h"

PreloadStage::PreloadStage(SDL_Renderer* render, ResourceManager *resMana, SoundEffectManager *sfxMana, Timer* timer)
{
    renderer = render;
    resourceManager = resMana;
    sfxManager = sfxMana;
    this->timer = timer;

    anistate = std::make_unique<AnimationState>();
    headAnimation = std::make_unique<FrameAnimation>(timer->getTotalTime(), 3, 3, true);

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
    }

    if (stageState == 2)
    {
        // 更新状态
        headAnimation->onUpdate(timer->getTotalTime(), *anistate);
        
    }
}

void PreloadStage::onRender()
{
    if (stageState == 2) {
        SDL_Texture* headTexture = resourceManager->GetTexture(1);
        SDL_Rect srcRect = { anistate->frameIndex * 256, 0,256, 256};
        SDL_RenderCopy(renderer, headTexture, &srcRect, NULL);
    }
}