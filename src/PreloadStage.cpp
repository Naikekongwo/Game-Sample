// PreloadStage.cpp
// 预加载页面的实现

#include "OpenCore/OpenCore.h"

PreloadStage::PreloadStage(SDL_Renderer* render, ResourceManager *resMana, SoundEffectManager *sfxMana, Timer* timer)
{
    renderer = render;
    resourceManager = resMana;
    sfxManager = sfxMana;
    this->timer = timer;

    spriteManager = std::make_unique<SpriteManager>();

    LoadResources();
}

void PreloadStage::LoadResources()
{
    resourceManager->LoadMusicAsync(0, TEST_MUSIC); // 加载背景音乐

    std::vector<std::pair<short, std::string>> assets = {
        {0, RES_GAME_ICON},
        {1, HEADS_ICON},
        {2, CHARA_TEXTURE},
        {3, LOADING_ICON}
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

        sfxManager->loadBGM(0); // BGM
        sfxManager->playBGM();
        sfxManager->setVolume(64); // 加载背景音乐

        stageState = 2;

        SDL_Log("PreloadStage: All resources loaded successfully.");

        Sprite* chara = spriteManager->CreateSprite(4, 2);

        chara->setAnchor(5); // 中心对齐

        chara->setPosition(640,360);

        chara->setScale(3.0, 3.0);

        std::shared_ptr<FrameAnimation> charaAnim = std::make_shared<FrameAnimation>(12, 3, true);
    
        chara->setTextureScale(48, 48);
        chara->setAnimation(charaAnim);
        chara->resetAnime(timer->getTotalTime());

        Sprite* loadingTag = spriteManager->CreateSprite(5, 3);

        loadingTag->setAnchor(9);

        loadingTag->setPosition(1280,720);

        loadingTag->setTextureScale(512, 256);
        
        loadingTag->setScale(0.4, 0.4);
        // Sprite* heads = spriteManager->CreateSprite(1, 1);
        // // 创建人头精灵
        // heads->setAnchor(5); // 中心对其
        // //设置坐标
        // heads->setPosition(640, 360);
        // heads->setTextureScale(256,256);
        // // 设置单帧尺寸
        // std::shared_ptr<FrameAnimation> headAnim = std::make_shared<FrameAnimation>(3, 3, true);
        // // 创建帧动画
        // heads->setAnimation(headAnim);
        // heads->resetAnime(timer->getTotalTime());

        // Sprite* heads1 = spriteManager->CreateSprite(2, 1);
        // // 创建人头精灵
        // heads1->setAnchor(4); // 中心对其
        // //设置坐标
        // heads1->setPosition(0, 360);
        // heads1->setTextureScale(256,256);
        // // 设置单帧尺寸
        // heads1->setAnimation(headAnim);
        // heads1->resetAnime(timer->getTotalTime());

        // Sprite* heads2 = spriteManager->CreateSprite(3, 1);
        // // 创建人头精灵
        // heads2->setAnchor(6); // 中心对其
        // //设置坐标
        // heads2->setPosition(1280, 360);
        // heads2->setTextureScale(256,256);
        // // 设置单帧尺寸
        // heads2->setAnimation(headAnim);
        // heads2->resetAnime(timer->getTotalTime());

    }

    if (stageState == 2)
    {
        // 更新状态
       spriteManager->onUpdate(timer->getTotalTime());
    }
}

void PreloadStage::onRender()
{
    if (stageState == 2)
    {
       spriteManager->onRender(renderer);
    }
}