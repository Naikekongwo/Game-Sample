#include "Eclipsea/Core/AudioManager.hpp"
#include "Eclipsea/Core/EclipseaTextures.hpp"
#include "Eclipsea/Core/GameSettings.hpp"
#include "Eclipsea/Eclipsea.hpp"
#include "Eclipsea/Gameplay/UI/MultiImageBoard.hpp"
#include "Eclipsea/Stage/MainStage.hpp"
#include "OpenCore.hpp"
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

void PreloadStage::onDestroy() { LOG("PreloadStage: onDestroy"); }

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

void PreloadStage::onRender() { Elements->onRender(); }

void PreloadStage::initializeComponents()
{
    // 1. 加载 UI（帧率 + 水波动画）
    buildLoadingUI();

    // 2. 初始化音乐（oceanwaves BGM）
    Eclipsea::AudioManager::getInstance().loadBGM("music_oceanwaves");
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
                buildTitle();
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
                title->changeTexture(MakeTexture(1, 1, "icon_studio"));
                title->Configure().ScaleR(0.3125f, 0.0f);
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
                title->changeTexture(MakeTexture(1, 1, "preload_title"));
                title->Configure().ScaleR(0.53f, 0.0f);
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
                // ShowConnector：新建独立的 connector 元素，从右上角滑入（x: 0
                // → 2700）， 与 MainStage 中继续滑向 6000 的 connector 在
                // x=2700 处视觉连续。
                auto connector =
                    UI<ImageBoard>("connector", 99, "img_connector", 1, 1);
                connector->Configure()
                    .ScaleR(0.0f, 1.0f)
                    .Anchor(AnchorPoint::TopRight)
                    .PositeR(0.0f, 0.0f);
                connector->Animate()
                    .Move(0, 0,
                          Eclipsea::GameSettings::getInstance().designX(2700),
                          0, 5.0f, false)
                    .Commit();
                Elements->PushElement(std::move(connector));
                return true;
            })
        .next(
            [this]() -> bool
            {
                auto *connector = Elements->find("connector");
                return connector && connector->isAnimeFinished();
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
    auto frameCounter = UI<FrameCounter>("frameCounter", 100, "9001", 0, 0);
    frameCounter->Configure().Sequence(true);
    frameCounter->Animate().Timer(6.0f).Commit();
    frameCounter->setFontName("9001");
    frameCounter->setFontSize(
        Eclipsea::GameSettings::getInstance().designFontSize(36));
    Elements->PushElement(std::move(frameCounter));

    // 水波动画（两帧纹理，多图层）
    auto animation = std::make_unique<MultiImageBoard>("animation", 10, 2);

    vector<unique_ptr<Texture>> animeList;
    animeList.push_back(std::move(MakeTexture(1, 5, "water_drops")));
    animeList.push_back(std::move(MakeTexture(1, 5, "water_waves")));

    animation->pushImageBoard(animeList);

    animation->Configure()
        .Anchor(AnchorPoint::Center)
        .ScaleR(0.208f, 0.208f * widthheight)
        .PositeR(0.5f, 0.5f)
        .Sequence(true);

    animation->ConfigureAt(0)
        .Anchor(AnchorPoint::Center)
        .ScaleR(1.0f, 1.0f)
        .PositeR(0.5f, 0.34f)
        .Sequence(true);
    animation->AnimateAt(0).Frame(5, 5, true).Commit();

    animation->ConfigureAt(1)
        .Anchor(AnchorPoint::Center)
        .ScaleR(1.0f, 1.0f)
        .PositeR(0.5f, 0.5f)
        .Sequence(true);
    animation->AnimateAt(1).Frame(5, 5, true).Commit();

    Elements->PushElement(std::move(animation));
}

void PreloadStage::buildTitle()
{
    // 标题（Waterrect 水面效果已暂移除，源代码保留于
    // Gameplay/Sprite/Waterrect）
    auto startTitle = UI<ImageBoard>("startTitle", 2, "icon_opencore", 1, 1);
    startTitle->Configure()
        .ScaleR(0.52f, 0.46f)
        .Anchor(AnchorPoint::Center)
        .PositeR(0.5f, 0.5f)
        .Sequence(false)
        .Alpha(0.0f);
    startTitle->Animate()
        .Fade(0.4f, 1.0f, 5.0f, false)
        .Scale(1.2f, 1.0f, 5.0f)
        .Commit();

    Elements->PushElement(std::move(startTitle));
}
