#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"
#include <memory>

SettingsStage::SettingsStage(Timer *timer, StageManager *sController)
{
    this->timer = timer;
    this->sController = sController;

    Elements = std::make_unique<ElementManager>();

    this->stageType = StageType::topStage;
}

void SettingsStage::onEnter()
{
    // 初始化设置页面，首先创建baseBackground
    auto baseBG =
        UI<BaseBackground>("set_background", 0, stone_background, 3, 3);

    baseBG->setNativeScale(255);

    baseBG->Configure()
        .Anchor(AnchorPoint::Center)
        .Posite(fullwidth * 0.5f, fullheight * 0.5f)
        .Scale(fullwidth * 0.5f, fullheight * 0.8f)
        .Sequence(true)
        .Follow(0)
        .Parent(nullptr);

    Elements->PushElement(std::move(baseBG));

    auto buttonBorder = UI<ImageBoard>("buttonBorders", 1, button_border, 1, 1);
    buttonBorder->Configure()
        .Scale(0.03f * 1.25f, 0.03f * 1.25f)
        .Posite(0.70f, 0.10f)
        .Anchor(AnchorPoint::Center)
        .Sequence(false)
        .Parent(nullptr);

    Elements->PushElement(std::move(buttonBorder));

    // 返回按钮
    auto backButton = UI<Button>("backButton", 1, img_BackButton, 1, 3);
    backButton->Configure()
        .Scale(0.03f * 0.8f, 0.03f * 0.8f)
        .Posite(0.70f, 0.10f)
        .Anchor(AnchorPoint::Center)
        .Sequence(false)
        .Parent(nullptr);

    // 点击回调
    backButton->setOnClick(
        [this]()
        {
            if (phase != SettingsPhase::Ready)
                return; // 防止多次点击
            phase = SettingsPhase::Exiting;
        });
    Elements->PushElement(std::move(backButton));
    // 将元素添加进入
}

void SettingsStage::onUpdate()
{
    Elements->onUpdate(timer->getTotalTime());
    // 刷新函数
    switch (phase)
    {
    case SettingsPhase::Entering:
    {
        auto bgc = Elements->find("set_background");
        if (bgc && bgc->isAnimeFinished())
            phase = SettingsPhase::Ready; // 准备完成的标志
        break;
    }
    case SettingsPhase::Ready:
        break;
    case SettingsPhase::Exiting:
        onExit();
        break;
    default:
        break;
    }
}

void SettingsStage::onExit() { sController->removeStage(this->stageType); }

bool SettingsStage::handlEvents(SDL_Event *event)
{
    Elements->handlEvents(*event, timer->getTotalTime());
    return true;
}

void SettingsStage::onDestroy() {}

void SettingsStage::onRender() { Elements->onRender(); }