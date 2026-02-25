#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"

PreloadStage::PreloadStage(Timer *timer, StageManager *sController)
{
    this->timer = timer;
    this->sController = sController;

    this->stageType = StageType::overlayStage;
    phase = PreloadPhase::WaitingForResource;

    Elements = std::make_unique<ElementManager>();
    pendingNextStage = false;
}

// 生命周期安全：进入舞台时初始化资源
void PreloadStage::onEnter()
{
    SDL_Log("PreloadStage: onEnter - starting resource load");

    LoadingState = OpenCoreManagers::ResManager.LoadResourcesFromJson(10001);
}

// 生命周期安全：退出舞台前清理
void PreloadStage::onExit()
{
    Elements->onDestroy();
    SDL_Log("PreloadStage: onExit - cleared elements");
}

// 生命周期安全：彻底销毁
void PreloadStage::onDestroy() { SDL_Log("PreloadStage: onDestroy"); }

// 事件传递：顶层拦截
bool PreloadStage::handlEvents(SDL_Event *event)
{
    if (phase >= PreloadPhase::BuildLoadingUI)
    {
        Elements->handlEvents(*event, timer->getTotalTime());
    }
    return true; // 拦截所有事件
}

// 分阶段更新函数
void PreloadStage::onUpdate()
{
    float totalTime = timer->getTotalTime();

    // 1️⃣ 先检查异步加载是否完成
    if (phase == PreloadPhase::WaitingForResource)
    {
        if (LoadingState.wait_for(std::chrono::milliseconds(0)) ==
            std::future_status::ready)
        {
            phase = PreloadPhase::BuildLoadingUI;

            if (DEBUG_MODE == DEBUG_MAP)
            {
                sController->removeStage(this->stageType);
                auto gameplay =
                    std::make_unique<GameplayStage>(timer, sController);
                sController->changeStage(std::move(gameplay));
            }
        }
    }

    // 2️⃣ 构建 UI 和动画
    if (phase == PreloadPhase::BuildLoadingUI)
    {
        buildLoadingUI();
        phase = PreloadPhase::WaitAnimation;
    }

    // 3️⃣ 等待动画完成
    if (phase == PreloadPhase::WaitAnimation)
    {
        Elements->onUpdate(totalTime);
        auto entry = Elements->find("frameCounter");
        if (entry && entry->isAnimeFinished())
        {
            Elements->removeElement("animation");
            phase = PreloadPhase::InitAudio;
        }
    }

    // 4️⃣ 初始化音乐
    if (phase == PreloadPhase::InitAudio)
    {
        auto &SFX = OpenCoreManagers::SFXManager;
        SFX.loadBGM(1001);
        SFX.playBGM();
        SFX.setVolume(30);
        phase = PreloadPhase::ShowTitle1;

        buildTitleAndWater();
    }

    // 5️⃣ 处理标题动画阶段
    if (phase == PreloadPhase::ShowTitle1 ||
        phase == PreloadPhase::ShowTitle2 ||
        phase == PreloadPhase::ShowConnector)
    {
        Elements->onUpdate(totalTime);
        handleTitleSequence();
    }

    // 6️⃣ 切换下一场景
    if (phase == PreloadPhase::Finished && !pendingNextStage)
    {
        pendingNextStage = true;
    }

    if (pendingNextStage)
    {
        pendingNextStage = false;

        if (DEBUG_MODE == DEBUG_MAP)
        {
            std::unique_ptr<GameplayStage> gameplayStage =
                std::make_unique<GameplayStage>(timer, sController);
            sController->changeStage(std::move(gameplayStage));
        }
        else
        {
            std::unique_ptr<MainStage> mainStage =
                std::make_unique<MainStage>(timer, sController);
            transferElementTo(mainStage.get(), "frameCounter");
            sController->changeStage(std::move(mainStage));
        }
    }
}

// 渲染
void PreloadStage::onRender()
{
    if (phase >= PreloadPhase::BuildLoadingUI)
    {
        Elements->onRender();
    }
}

////////////////////////////////////////////////////////////////////////////////
// 内部辅助函数
////////////////////////////////////////////////////////////////////////////////

void PreloadStage::buildLoadingUI()
{
    auto frameCounter = UI<FrameCounter>("frameCounter", 100, 0, 0, 0);
    frameCounter->Configure().Sequence(true);
    frameCounter->Animate().Timer(6.0f).Commit();
    Elements->PushElement(std::move(frameCounter));

    std::vector<std::unique_ptr<Texture>> animeList;
    animeList.push_back(std::move(MakeTexture(1, 5, water_drops)));
    animeList.push_back(std::move(MakeTexture(1, 5, water_waves)));

    auto animation = UI<MultiImageBoard>("animation", 10, 2, 0, 0);
    animation->pushImageBoard(animeList);
    animation->Configure()
        .Anchor(AnchorPoint::Center)
        .Scale(0.208f, 0.117f)
        .Posite(0.5f, 0.28f)
        .Sequence(true);

    animation->ConfigureAt(0)
        .Anchor(AnchorPoint::Center)
        .Scale(fullwidth, fullheight)
        .Posite(0.5f, 0.28f)
        .Sequence(true);
    animation->AnimateAt(0).Frame(5, 5, true).Commit();

    animation->ConfigureAt(1)
        .Anchor(AnchorPoint::Center)
        .Scale(fullwidth, fullheight)
        .Posite(0.5f, 0.4f)
        .Sequence(true);
    animation->AnimateAt(1).Frame(5, 5, true).Commit();

    Elements->PushElement(std::move(animation));
}

void PreloadStage::buildTitleAndWater()
{
    auto startTitle = UI<ImageBoard>("startTitle", 2, icon_opencore, 1, 1);
    startTitle->Configure()
        .Scale(0.52f, 0.26f)
        .Anchor(AnchorPoint::Center)
        .Posite(0.5f, 0.28125f)
        .Sequence(false)
        .Alpha(0.0f);
    startTitle->Animate().Fade(0.4f, 1.0f, 5.0f, false).Commit();

    auto waterRect = std::make_unique<Waterrect>(
        "water", 1,
        std::make_unique<Texture>(
            1, 1, OpenCoreManagers::ResManager.GetTexture(2017)));

    waterRect->Configure()
        .Anchor(AnchorPoint::TopCenter)
        .Parent(nullptr)
        .Posite(0.5 * fullwidth, 0.5 * fullheight)
        .Scale(1.0 * fullwidth, 0.5 * fullheight);

    auto wave = std::make_unique<Wave>();
    wave->insertWave(WaveInfo(50, 1200, 0.02, 0, 1.0, 0.0));
    wave->insertWave(WaveInfo(18, 600, 0.12, 1.0, 0.8, 0.6));
    wave->insertWave(WaveInfo(4.0, 180, 0., 0.5, -0.3, 0.95));

    waterRect->setWave(std::move(wave));
    waterRect->setDetail(50);

    std::vector<float> rSets{0.0f,
                             0.5f * fullheight,
                             1.0f,
                             0.5f * fullheight,
                             -0.25f * fullwidth,
                             1.0f * fullheight,
                             1.25f * fullwidth,
                             1.0f * fullheight};
    waterRect->SpecialDraw(true, rSets);

    Elements->PushElement(std::move(waterRect));
    Elements->PushElement(std::move(startTitle));
}

void PreloadStage::handleTitleSequence()
{
    auto Title = Elements->find("startTitle");
    if (!Title || !Title->isAnimeFinished())
        return;

    switch (phase)
    {
    case PreloadPhase::ShowTitle1:
        Title->setTransparency(0.0f);
        Title->changeTexture(MakeTexture(1, 1, icon_studio));
        Title->Configure().Scale(0.3125f, 0.3125f);
        Title->setSequential(true);
        Title->Animate().Fade(0.0f, 1.0f, 2.0f, false).Timer(3.0f).Commit();
        phase = PreloadPhase::ShowTitle2;
        break;

    case PreloadPhase::ShowTitle2:
        Title->setTransparency(0.0f);
        Title->changeTexture(MakeTexture(1, 1, preload_title));
        Title->Configure().Scale(0.53f, 0.13f);
        Title->setSequential(true);
        Title->Animate().Fade(0.0f, 1.0f, 2.0f, false).Timer(10.0f).Commit();
        phase = PreloadPhase::ShowConnector;
        break;

    case PreloadPhase::ShowConnector:
    {
        auto connector = UI<ImageBoard>("connector", 99, img_connector, 1, 1);
        connector->Configure()
            .Scale(fullwidth, fullheight)
            .Anchor(AnchorPoint::TopRight)
            .Posite(0.0f, 0.0f);
        connector->Animate().Move(0, 0, 1920, 0, 5.0f, false).Commit();
        Elements->PushElement(std::move(connector));
        phase = PreloadPhase::Finished;
        break;
    }

    default:
        break;
    }
}