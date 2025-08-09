// PreloadStage.cpp
// 预加载页面的实现

#include "OpenCore/OpenCore.hpp"

PreloadStage::PreloadStage(Timer* timer, StageController* sController)
{
    this->timer = timer;
    Elements = std::make_unique<ElementManager>();

    this->sController = sController;
    LoadResources();
}

void PreloadStage::LoadResources()
{
    LoadingState = OpenCoreManagers::ResManager.LoadResourcesFromJson(10001);
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
    bool preload = false;

    if (LoadingState.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
        preload = true;
    }

    if (preload && stageState == 0)
    {
        stageState = 1;

        OpenCoreManagers::SFXManager.loadBGM(1001); // BGM
        OpenCoreManagers::SFXManager.playBGM();
        OpenCoreManagers::SFXManager.setVolume(30); // 加载背景音乐

        stageState = 2;

        SDL_Log("PreloadStage: All resources loaded successfully.");
        
        std::unique_ptr<ImageBoard> startTitle = std::make_unique<ImageBoard>("startTitle", 0, new Texture(1, 1, OpenCoreManagers::ResManager.GetTexture(2003)));
        // 创建了加载界面的标题控件

        // 部署各项属性
        startTitle->Configure().Scale(1000,500).Anchor(AnchorPoint::Center).Posite(960, 540).Sequence(false);
        // 配置动画属性
        startTitle->Animate().Fade(0.4f, 1.0f, 5.0f, false).Commit();

        std::unique_ptr<ImageBoard> oceanBackground = std::make_unique<ImageBoard>("background", -1, new Texture(2, 1, OpenCoreManagers::ResManager.GetTexture(2001)));
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

    if (stageState >= 2 && stageState <=4)
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
                    Title->changeTexture(new Texture(1,1, OpenCoreManagers::ResManager.GetTexture(2004)));
                    Title->Configure().Scale(600,600);
                    Title->setSequential(true);
                    Title->Animate().Fade(0.0f, 1.0f, 2.0f, false).Timer(3.0f).Commit();
                    stageState = 3;
                    return;
                }

                if(stageState == 3)
                {
                    Title->changeTexture(new Texture(1,1, OpenCoreManagers::ResManager.GetTexture(2002)));
                    Title->Configure().Scale(1024,256);
                    Title->setSequential(true);
                    Title->Animate().Fade(0.0f, 1.0f, 2.0f, false).Timer(10.0f).Commit();
                    stageState = 4;
                    return;
                }

                if(stageState == 4)
                {
                    std::unique_ptr<ImageBoard> connector = std::make_unique<ImageBoard>("connector", 99, new Texture(1,1,OpenCoreManagers::ResManager.GetTexture(2005)));
                    // 创建了遮罩和背景的资源

                    connector->Configure().Scale(1920,1080).Anchor(AnchorPoint::TopRight).Posite(0,0);
                    connector->Animate().Move(0,0,1920,0,5.0f, false).Commit();

                    Elements->PushElement(std::move(connector));
                    stageState = 5;
                    return;
                }

            }
        }
    }

    if(stageState == 5)
    {
        // 更新状态
        Elements->onUpdate(timer->getTotalTime());
        
        auto connector = Elements->find("connector");

        if(connector && connector->isAnimeFinished())
        {
            // 进入生命周期末期销毁！
            std::unique_ptr<MainStage> mainStage  = std::make_unique<MainStage>(timer, sController);
            sController->changeStage(std::move(mainStage));
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