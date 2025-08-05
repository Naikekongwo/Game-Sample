// PreloadStage.cpp
// 预加载页面的实现

#include "OpenCore/OpenCore.h"

PreloadStage::PreloadStage(Timer *timer)
{
    this->timer = timer;
    Elements = std::make_unique<ElementManager>();
    LoadResources();
}

void PreloadStage::LoadResources()
{
    OpenCoreManagers::ResManager.LoadMusicAsync(0, OCEAN_WAVES); // 加载背景音乐

    std::vector<std::pair<short, std::string>> assets = {
        {0, RES_GAME_ICON},
        {1, HEADS_ICON},
        {2, CHARA_TEXTURE},
        {3, LOADING_ICON},
        {4, GAMESTART_ICON},
        {5, OCEAN_BACK},
        {6, "assets/ui/loading.png"}};

    for (const auto &[id, path] : assets)
    {
        TextureLoadTask task;
        task.id = id;
        task.path = path;
        task.future = OpenCoreManagers::ResManager.LoadTextureAsync(id, path);
        textureTasks.push_back(std::move(task));
    }
    SDL_Log("PreloadStage: Resources loading started.");
}

bool PreloadStage::handlEvents(SDL_Event *event)
{

    Elements->handlEvents(*event, timer->getTotalTime());
    return true;
    // 默认成功
}

void PreloadStage::onUpdate()
{
    bool allReady = true;
    for (auto &task : textureTasks)
    {
        if (!task.isFinished)
        {
            if (task.future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
            {
                try
                {
                    task.future.get();
                }
                catch (const std::exception &e)
                {
                    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Exception in async load: %s", e.what());
                }
                task.isFinished = true;
            }
            else
            {
                allReady = false;
            }
        }
    }

    if (allReady && stageState == 0)
    {
        stageState = 1;

        OpenCoreManagers::SFXManager.loadBGM(0); // BGM
        OpenCoreManagers::SFXManager.playBGM();
        OpenCoreManagers::SFXManager.setVolume(64); // 加载背景音乐

        stageState = 2;

        SDL_Log("PreloadStage: All resources loaded successfully.");
        std::unique_ptr<Texture> imgTex = std::make_unique<Texture>(1, 1, OpenCoreManagers::ResManager.GetTexture(6));
        std::unique_ptr<ImageBoard> imgBd = std::make_unique<ImageBoard>(1, 0, std::move(imgTex));
        // 创建了控件

        imgBd->setScale(512, 256);
        imgBd->setAnchor(AnchorPoint::Center);
        imgBd->setPosition(640, 360);

        std::shared_ptr<FadeAnimation> fade0 = std::make_shared<FadeAnimation>(0.4f, 1.0f, 5.0f, false);
        imgBd->PushAnimation(1, fade0);

        std::unique_ptr<Texture> imgTex1 = std::make_unique<Texture>(2, 1, OpenCoreManagers::ResManager.GetTexture(5));
        std::unique_ptr<ImageBoard> imgBd1 = std::make_unique<ImageBoard>(2, -1, std::move(imgTex1));
        // 创建了控件

        imgBd1->setScale(1280, 720);
        imgBd1->setAnchor(AnchorPoint::Center);
        imgBd1->setPosition(640, 360);

        std::shared_ptr<FadeAnimation> fade = std::make_shared<FadeAnimation>(0.0f, 1.0f, 10.0f, false);
        imgBd1->PushAnimation(1, fade);

        Elements->PushElement(std::move(imgBd));
        Elements->PushElement(std::move(imgBd1));
    }

    if (stageState == 2)
    {
        // 更新状态
        Elements->onUpdate(timer->getTotalTime());
    }
}

void PreloadStage::onRender()
{
    if (stageState == 2)
    {
        Elements->onRender();
    }
}