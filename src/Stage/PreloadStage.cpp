#include "Eclipsea/Eclipsea.hpp"
#include "Eclipsea/Stage/MainStage.hpp"
#include "Eclipsea/Gameplay/UI/MultiImageBoard.hpp"
#include "Eclipsea/Gameplay/Sprite/Waterrect.hpp"
#include "OpenCore.hpp"
#include "Core/Math/OpenCore_Wave.hpp"
#include "Eclipsea/Core/AudioManager.hpp"
#include "Eclipsea/Core/EclipseaTextures.hpp"
#include <memory>

// PreloadStage（新 OpenCore）：使用 SequentialPipeline 顺序驱动
PreloadStage::PreloadStage()
{
    // OverlayStage 基类已设置 stageType 与 Elements
}

void PreloadStage::onEnter()
{
    LOG("PreloadStage: onEnter");
    initializeComponents();
}

void PreloadStage::onExit()
{
    Elements->onDestroy();
    LOG("PreloadStage: onExit - cleared elements");
}

void PreloadStage::onDestroy()
{
    LOG("PreloadStage: onDestroy");
}

bool PreloadStage::parseEvents(Event *event)
{
    Elements->parseEvents(event, timer->getTotalTime());
    return true;
}

void PreloadStage::onUpdate()
{
    Elements->onUpdate(timer->getTotalTime());
    pipeline.onUpdate(timer->getTotalTime());
}

void PreloadStage::onRender()
{
    Elements->onRender();
}

void PreloadStage::initializeComponents()
{
    // 1. 加载 UI（帧率 + 水波动画）
    buildLoadingUI();

    // 2. 初始化音乐（oceanwaves BGM）
    Eclipsea::AudioManager::getInstance().loadBGM(music_oceanwaves);
    Eclipsea::AudioManager::getInstance().playBGM();
    Eclipsea::AudioManager::getInstance().setVolume(30);

    // 3. 顺序执行标题动画序列
    pipeline
        .next(
            [this]() -> bool
            {
                auto *animation = Elements->find("animation");
                return !animation || animation->isAnimeFinished();
            })
        .next(
            [this]() -> bool
            {
                Elements->removeElement("animation");
                buildTitleAndWater();
                return true;
            })
        .next(
            [this]() -> bool
            {
                auto *title = Elements->find("startTitle");
                return title && title->isAnimeFinished();
            })
        .next(
            [this]() -> bool
            {
                // ShowTitle1：切换到 studio 图标
                auto *title = Elements->find("startTitle");
                if (!title)
                    return true;
                title->setTransparency(0.0f);
                title->changeTexture(MakeTexture(1, 1, icon_studio));
                title->Configure().Scale(0.3125f, 0.0f);
                title->setSequential(true);
                title->Animate()
                    .SubStart(true)
                    ->Fade(0.0f, 1.0f, 2.0f, false)
                    .SubEnd()
                    .Timer(3.0f)
                    .Commit();
                return true;
            })
        .next(
            [this]() -> bool
            {
                auto *title = Elements->find("startTitle");
                return title && title->isAnimeFinished();
            })
        .next(
            [this]() -> bool
            {
                // ShowTitle2：切换到 Preload_Title
                auto *title = Elements->find("startTitle");
                if (!title)
                    return true;
                title->setTransparency(0.0f);
                title->changeTexture(MakeTexture(1, 1, preload_title));
                title->Configure().Scale(0.53f, 0.0f);
                title->setSequential(true);
                title->Animate()
                    .Fade(0.0f, 1.0f, 2.0f, false)
                    .Timer(10.0f)
                    .Commit();
                return true;
            })
        .next(
            [this]() -> bool
            {
                auto *title = Elements->find("startTitle");
                return title && title->isAnimeFinished();
            })
        .next(
            [this]() -> bool
            {
                // ShowConnector：切换到连接器动画
                auto *title = Elements->find("startTitle");
                if (!title)
                    return true;
                title->setTransparency(0.0f);
                title->changeTexture(MakeTexture(1, 1, img_connector));
                title->Configure().Scale(0.53f, 0.0f);
                title->setSequential(true);
                title->Animate()
                    .Fade(0.0f, 1.0f, 2.0f, false)
                    .Timer(5.0f)
                    .Commit();
                return true;
            })
        .next(
            [this]() -> bool
            {
                auto *title = Elements->find("startTitle");
                return title && title->isAnimeFinished();
            })
        .next(
            [this]() -> bool
            {
                // 过渡到主菜单
                auto mainStage = std::make_unique<MainStage>();
                transferElementTo(mainStage.get(), "frameCounter");
                sController->changeStage(std::move(mainStage));
                return true;
            });
}

void PreloadStage::buildLoadingUI()
{
    auto frameCounter =
        UI<FrameCounter>("frameCounter", 100, "9001", 0, 0);
    frameCounter->Configure().Sequence(true);
    frameCounter->Animate().Timer(6.0f).Commit();
    frameCounter->setFontName("9001");
    frameCounter->setFontSize(36);
    Elements->PushElement(std::move(frameCounter));

    // 水波动画（两帧纹理，多图层）
    auto animation = std::make_unique<MultiImageBoard>("animation", 10, 2);

    vector<unique_ptr<Texture>> animeList;
    animeList.push_back(std::move(MakeTexture(1, 5, water_drops)));
    animeList.push_back(std::move(MakeTexture(1, 5, water_waves)));

    animation->pushImageBoard(animeList);

    animation->Configure()
        .Anchor(AnchorPoint::Center)
        .Scale(0.208f, 0.208f * widthheight)
        .Posite(0.5f, 0.5f)
        .Sequence(true);

    animation->ConfigureAt(0)
        .Anchor(AnchorPoint::Center)
        .Scale(1.0f, 1.0f)
        .Posite(0.5f, 0.34f)
        .Sequence(true);
    animation->AnimateAt(0).Frame(5, 5, true).Commit();

    animation->ConfigureAt(1)
        .Anchor(AnchorPoint::Center)
        .Scale(1.0f, 1.0f)
        .Posite(0.5f, 0.5f)
        .Sequence(true);
    animation->AnimateAt(1).Frame(5, 5, true).Commit();

    Elements->PushElement(std::move(animation));
}

void PreloadStage::buildTitleAndWater()
{
    auto startTitle = UI<ImageBoard>("startTitle", 2, "icon_opencore", 1, 1);
    startTitle->Configure()
        .Scale(0.52f, 0.46f)
        .Anchor(AnchorPoint::Center)
        .Posite(0.5f, 0.5f)
        .Sequence(false)
        .Alpha(0.0f)
        .Follow(20);
    startTitle->Animate()
        .Fade(0.4f, 1.0f, 5.0f, false)
        .Scale(1.2f, 1.0f, 5.0f)
        .Commit();

    // 水面效果（Waterrect）
    auto waterRect = std::make_unique<Waterrect>(
        "water", 1,
        std::make_unique<Texture>(
            1, 1, EclipseaTextures::getInstance().getTexture(oriwater)));

    waterRect->Configure()
        .Anchor(AnchorPoint::TopCenter)
        .Parent(nullptr)
        .Posite(0.5f, 0.5f)
        .Scale(1.0f, 0.5f);

    auto wave = std::make_unique<Wave>();
    wave->insertWave(WaveInfo(50, 1200, 0.04, 0, 1.0, 0.0));
    wave->insertWave(WaveInfo(18, 600, 0.24, 1.0, 0.8, 0.6));
    wave->insertWave(WaveInfo(4.0, 180, 0.0, 0.5, -0.3, 0.95));

    waterRect->setWave(std::move(wave));
    waterRect->setDetail(50);

    vector<float> rSets{0.0f,
                        0.5f * heightfactor,
                        1.0f,
                        0.5f * heightfactor,
                        -0.25f * fullwidth,
                        1.0f * heightfactor,
                        1.25f * fullwidth,
                        1.0f * heightfactor};
    waterRect->SpecialDraw(true, rSets);

    Elements->PushElement(std::move(waterRect));
    Elements->PushElement(std::move(startTitle));
}
