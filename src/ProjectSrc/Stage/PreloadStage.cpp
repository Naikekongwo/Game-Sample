// PreloadStage.cpp
// 预加载页面的实现

#include "OpenCore/OpenCore.hpp"
#include "Union.hpp"

PreloadStage::PreloadStage(Timer *timer, StageController *sController)
{
    this->timer = timer;
    Elements = std::make_unique<ElementManager>();

    this->sController = sController;

    this->stageType = StageType::baseStage;
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

    // 简化引用的措施
    auto &SFX = OpenCoreManagers::SFXManager;

    if (LoadingState.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
    {
        preload = true;
    }

    if (preload && stageState == 0)
    {
        stageState = 1;

        SFX.loadBGM(1001); // BGM
        SFX.playBGM();
        SFX.setVolume(30); // 加载背景音乐

        stageState = 2;

        SDL_Log("PreloadStage: All resources loaded successfully.");

        auto startTitle = UI<ImageBoard>("startTitle", 0, 2003, 1, 1);
        auto oceanBackground = UI<ImageBoard>("background", -1, 2001, 2, 1);
        // 创建了加载界面的标题控件
        // 创建了背景控件

        // 部署各项属性
        startTitle->Configure().Scale(1000, 500).Anchor(AnchorPoint::Center).Posite(960, 540).Sequence(false);
        oceanBackground->Configure().Scale(1920, 1080).Anchor(AnchorPoint::Center).Posite(960, 540);
        startTitle->Animate().Fade(0.4f, 1.0f, 5.0f, false).Commit();
        oceanBackground->Animate().Fade(0.0f, 1.0f, 10.0f, false).Timer(5.0f).Fade(1.0f, 0.0f, 5.0f, false).Commit();

        auto frameCounter = UI<FrameCounter>("frameCounter", 100, 0, 0, 0);

        // 使得背景的动画执行顺序变为顺序执行
        oceanBackground->setSequential(true);

        Elements->PushElement(std::move(startTitle));
        Elements->PushElement(std::move(oceanBackground));
        Elements->PushElement(std::move(frameCounter));
    }

    if (stageState >= 2 && stageState <= 4)
    {
        // 更新状态
        Elements->onUpdate(timer->getTotalTime());

        auto Title = Elements->find("startTitle");
        if (Title)
        {
            if (Title->isAnimeFinished())
            {

                if (stageState == 2)
                {
                    Title->changeTexture(new Texture(1, 1, OpenCoreManagers::ResManager.GetTexture(2004)));
                    Title->Configure().Scale(600, 600);
                    Title->setSequential(true);
                    Title->Animate().Fade(0.0f, 1.0f, 2.0f, false).Timer(3.0f).Commit();
                    stageState = 3;
                    return;
                }

                if (stageState == 3)
                {
                    Title->changeTexture(new Texture(1, 1, OpenCoreManagers::ResManager.GetTexture(2002)));
                    Title->Configure().Scale(1024, 256);
                    Title->setSequential(true);
                    Title->Animate().Fade(0.0f, 1.0f, 2.0f, false).Timer(10.0f).Commit();
                    stageState = 4;
                    return;
                }

                if (stageState == 4)
                {
                    auto connector = UI<ImageBoard>("connector", 99, 2005, 1, 1);
                    // 创建了遮罩和背景的资源

                    connector->Configure().Scale(1920, 1080).Anchor(AnchorPoint::TopRight).Posite(0, 0);
                    connector->Animate().Move(0, 0, 1920, 0, 5.0f, false).Commit();

                    Elements->PushElement(std::move(connector));
                    stageState = 5;
                    return;
                }
            }
        }
    }

    if (stageState == 5)
    {
        // 更新状态
        Elements->onUpdate(timer->getTotalTime());

        auto connector = Elements->find("connector");

        if (connector && connector->isAnimeFinished())
        {
            // 进入生命周期末期销毁！
            if (DEBUG_MODE == DEBUG_MAP)
            {
                std::unique_ptr<GameplayStage> gameplayStage = std::make_unique<GameplayStage>(timer, sController);
                sController->changeStage(std::move(gameplayStage));
            }
            else
            {
                std::unique_ptr<MainStage> mainStage = std::make_unique<MainStage>(timer, sController);
                transferElementTo(mainStage.get(), "frameCounter");
                sController->changeStage(std::move(mainStage));
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