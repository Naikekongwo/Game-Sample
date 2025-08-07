// PreloadStage.cpp
// 预加载页面的实现

#include "OpenCore/OpenCore.hpp"

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
        {6, "assets/ui/Preload_Title.png"}};

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
        OpenCoreManagers::SFXManager.setVolume(30); // 加载背景音乐

        stageState = 2;

        SDL_Log("PreloadStage: All resources loaded successfully.");
        
        std::unique_ptr<ImageBoard> startTitle = std::make_unique<ImageBoard>("startTitle", 0, new Texture(1, 1, OpenCoreManagers::ResManager.GetTexture(6)));
        // 创建了加载界面的标题控件

        // 部署各项属性
        startTitle->Configure().Scale(1024,256).Anchor(AnchorPoint::Center).Posite(960, 540).Sequence(false);
        // 配置动画属性
        startTitle->Animate().Fade(0.4f, 1.0f, 5.0f, false).Scale(1.3f, 1.1f, 5.0f, false).Commit();

        std::unique_ptr<ImageBoard> oceanBackground = std::make_unique<ImageBoard>("background", -1, new Texture(2, 1, OpenCoreManagers::ResManager.GetTexture(5)));
        // 创建了背景控件
        
        // 部署背景的各项属性
        oceanBackground->Configure().Scale(1920, 1080).Anchor(AnchorPoint::Center).Posite(960, 540);
        // 配置背景的动画
        oceanBackground->Animate().Fade(0.0f, 1.0f, 10.0f, false).Timer(5.0f).Fade(1.0f,0.0f,5.0f, false).Commit();

        // 使得背景的动画执行顺序变为顺序执行
        oceanBackground->setSequential(true);

        Elements->PushElement(std::move(startTitle));
        Elements->PushElement(std::move(oceanBackground));
    }

    if (stageState >= 2)
    {
        // 更新状态
        Elements->onUpdate(timer->getTotalTime());

        auto Title = Elements->find("startTitle");
        if(Title)
        {
            if(Title->isAnimeFinished())
            {
                if(stageState == 2)
                {
                    Title->Animate().Scale(1.1f, 1.0f, 5.0f, false).Commit();
                    stageState = 3;
                    return;
                }

                if(stageState == 3)
                {
                    Title->changeTexture(new Texture(1,1, OpenCoreManagers::ResManager.GetTexture(3)));
                    Title->Configure().Scale(1000,500);
                    Title->setSequential(true);
                    Title->Animate().Fade(0.0f, 1.0f, 2.0f, false).Timer(3.0f).Commit();
                    stageState = 4;
                    return;
                }

                if(stageState == 4)
                {
                    Title->changeTexture(new Texture(1,1, OpenCoreManagers::ResManager.GetTexture(1)));
                    Title->Configure().Scale(600,600);
                    Title->setSequential(true);
                    Title->Animate().Fade(0.0f, 1.0f, 2.0f, false).Timer(3.0f).Commit();
                    stageState = 5;
                    return;
                }

            }
        }
    }
}

void PreloadStage::onRender()
{
    if (stageState >= 2)
    {
        Elements->onRender();
    }
}