// PreloadStage.cpp
// 预加载页面的实现

#include "OpenCore/OpenCore.hpp"
#include "Eclipsea/Eclipsea.hpp"

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
        // 预加载已经完毕
        // 第二段加载开始的位置
    }

    if (preload && stageState == 0)
    {
        auto frameCounter = UI<FrameCounter>("frameCounter", 100, NULL, NULL, NULL);

        frameCounter->Configure().Sequence(true);
        frameCounter->Animate().Timer(6.0f).Commit();

        Elements->PushElement(std::move(frameCounter));
        
        stageState = 1;

        std::vector<std::unique_ptr<Texture>> animeList;


        animeList.push_back(std::move(MakeTexture(1,5,2013)));
        animeList.push_back(std::move(MakeTexture(1,5,2014)));
        auto animation = UI<MultiImageBoard>("animation", 10, 2, 0, 0);

        animation->pushImageBoard(animeList);
        animation->Configure().Anchor(AnchorPoint::Center).Scale(0.208f, 0.117f).Posite(0.5f, 0.28f).Sequence(true);
        animation->ConfigureAt(0).Anchor(AnchorPoint::Center).Scale(1.0f, 0.56f).Posite(0.5f, 0.28f).Sequence(true);
        animation->AnimateAt(0).Frame(5,5,true).Commit();

        animation->ConfigureAt(1).Anchor(AnchorPoint::Center).Scale(1.0f, 0.56f).Posite(0.5f, 0.4f).Sequence(true);
        animation->AnimateAt(1).Frame(5,5,true).Commit();

        Elements->PushElement(std::move(animation));

        return;
    }

    if(preload && stageState == 1)
    {
        Elements->onUpdate(timer->getTotalTime());
        auto entry = Elements->find("frameCounter");
        if(entry && entry->isAnimeFinished())
        {
            Elements->removeElement("animation");
            stageState = 2;
            return;
        }
    }

    if (preload && stageState == 2)
    {
        SFX.loadBGM(1001); // BGM
        SFX.playBGM();
        SFX.setVolume(30); // 加载背景音乐

        stageState = 3;

        SDL_Log("PreloadStage: All resources loaded successfully.");

        auto startTitle = UI<ImageBoard>("startTitle", 1, 2003, 1, 1);
        auto oceanBackground = UI<ImageBoard>("background", 0, 2001, 2, 1);
        // 创建了加载界面的标题控件
        // 创建了背景控件

        // 部署各项属性
        startTitle->Configure().Scale(0.52f, 0.26f).Anchor(AnchorPoint::Center).Posite(0.5f, 0.28125f).Sequence(false);
        oceanBackground->Configure().Scale(1.0f, 0.56f).Anchor(AnchorPoint::Center).Posite(0.5f, 0.28125f);
        startTitle->Animate().Fade(0.4f, 1.0f, 5.0f, false).Commit();
        oceanBackground->Animate().Fade(0.0f, 1.0f, 10.0f, false).Timer(5.0f).Fade(1.0f, 0.0f, 5.0f, false).Commit();


        // 使得背景的动画执行顺序变为顺序执行
        oceanBackground->setSequential(true);

        Elements->PushElement(std::move(startTitle));
        Elements->PushElement(std::move(oceanBackground)); 
    }

    if (stageState >= 3 && stageState <= 5)
    {
        // 更新状态
        Elements->onUpdate(timer->getTotalTime());

        auto Title = Elements->find("startTitle");
        if (Title)
        {
            if (Title->isAnimeFinished())
            {

                if (stageState == 3)
                {
                    Title->changeTexture(MakeTexture(1,1,2004));
                    Title->Configure().Scale(0.3125f, 0.3125f);
                    Title->setSequential(true);
                    Title->Animate().Fade(0.0f, 1.0f, 2.0f, false).Timer(3.0f).Commit();
                    stageState = 4;
                    return;
                }

                if (stageState == 4)
                {
                    Title->changeTexture(MakeTexture(1,1,2002));
                    Title->Configure().Scale(0.53f, 0.13f);
                    Title->setSequential(true);
                    Title->Animate().Fade(0.0f, 1.0f, 2.0f, false).Timer(10.0f).Commit();
                    stageState = 5;
                    return;
                }

                if (stageState == 5)
                {
                    auto connector = UI<ImageBoard>("connector", 99, 2005, 1, 1);
                    // 创建了遮罩和背景的资源

                    connector->Configure().Scale(1.0f, 0.56f).Anchor(AnchorPoint::TopRight).Posite(0.0f, 0.0f);
                    connector->Animate().Move(0, 0, 1920, 0, 5.0f, false).Commit();

                    Elements->PushElement(std::move(connector));
                    stageState = 6;
                    return;
                }
            }
        }
    }

    if (stageState == 6)
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
    if (stageState >= 1)
    {
        Elements->onRender();
    }
}