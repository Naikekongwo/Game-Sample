#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"
#include <functional>
#include <memory>

MainStage::MainStage(Timer *timer, StageManager *sController)
{
    this->timer = timer;

    this->sController = sController;

    Elements = std::make_unique<ElementManager>();

    this->stageType = StageType::overlayStage;

    setupBackground();
    setupTitle();
    setupButtons();
}

void MainStage::setupBackground()
{
    auto bg = UI<ImageBoard>("background", 0, background_main, 1, 1);
    bg->Configure()
        .Anchor(AnchorPoint::Center)
        .Posite(0.5 * fullwidth, 0.5 * fullheight)
        .Scale(fullwidth, fullheight);
    Elements->PushElement(std::move(bg));
    auto connector = UI<ImageBoard>("connector", 99, img_connector, 1, 1);
    connector->Configure()
        .Anchor(AnchorPoint::TopLeft)
        .Posite(0, 0)
        .Scale(fullwidth, fullheight);
    connector->Animate().Move(0, 0, 1920, 0, 5.f, false).Commit();
    Elements->PushElement(std::move(connector));
}

void MainStage::setupTitle()
{
    auto title = UI<ImageBoard>("mainTitle", 1, main_title, 1, 1);
    title->Configure()
        .Anchor(AnchorPoint::TopLeft)
        .Parent(nullptr)
        .Posite(0.08333f, 0.0677f)
        .Scale(0.365f, 0.13f)
        .Sequence(true);
    Elements->PushElement(std::move(title));

    auto copyright_icon = UI<ImageBoard>("copyrights", 1, copyright, 1, 1);
    copyright_icon->Configure()
        .Anchor(AnchorPoint::BottomLeft)
        .Parent(nullptr)
        .Posite(0.0167f, 0.546f)
        .Scale(0.258f, 0.0276f)
        .Sequence(true)
        .Alpha(0.0f);
    copyright_icon->Animate().Timer(5.0f).Fade(0.0f, 1.0f, 5.0f).Commit();

    Elements->PushElement(std::move(copyright_icon));
}

void MainStage::setupButtons()
{
    // 创建按钮
    auto startButton = UI<Button>("startButton", 1, img_StartButton, 1, 3);
    auto continueButton = UI<Button>("continueButton", 1, img_ContButton, 1, 3);
    auto settingButton = UI<Button>("settingButton", 1, img_SettButton, 1, 3);

    // 配置位置与缩放
    startButton->Configure()
        .Anchor(AnchorPoint::TopLeft)
        .Parent(nullptr)
        .Posite(0.755f, 0.383f)
        .Scale(0.156f, 0.0416f)
        .Sequence(true);

    continueButton->Configure()
        .Anchor(AnchorPoint::TopLeft)
        .Parent(nullptr)
        .Posite(0.79f, 0.4375f)
        .Scale(0.156f, 0.0416f)
        .Sequence(true);

    settingButton->Configure()
        .Anchor(AnchorPoint::TopLeft)
        .Parent(nullptr)
        .Posite(0.825f, 0.492f)
        .Scale(0.156f, 0.0416f)
        .Sequence(true);

    // 回调
    startButton->setOnClick(
        [this]()
        {
            auto gameplay = std::make_unique<GameplayStage>(timer, sController);
            sController->changeStage(std::move(gameplay));
        });

    continueButton->setOnClick(
        [this]()
        {
            SDL_Log("Continue button clicked");
            // TODO: 可以加读取存档逻辑
        });

    settingButton->setOnClick(
        [this]()
        {
            SDL_Log("Setting button clicked");
            // TODO: 打开设置界面
        });

    // 推入元素管理器
    Elements->PushElement(std::move(startButton));
    Elements->PushElement(std::move(continueButton));
    Elements->PushElement(std::move(settingButton));
}

void MainStage::onEnter()
{
    // 播放进入动画、BGM
    phase = MainStagePhase::Idle;
}

void MainStage::onExit()
{
    // 停止动画、音效
}

void MainStage::onUpdate() { Elements->onUpdate(timer->getTotalTime()); }

void MainStage::onRender() { Elements->onRender(); }

bool MainStage::handlEvents(SDL_Event *event)
{
    Elements->handlEvents(*event, timer->getTotalTime());
    return true;
}