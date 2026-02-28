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
    auto Container =
        UI<BaseBackground>("set_background", 0, stone_background, 3, 3);

    Container->setNativeScale(255);

    Container->Configure()
        .Anchor(AnchorPoint::Center)
        .Posite(fullwidth * 0.5f, fullheight * 0.5f)
        .Scale(fullwidth * 0.6f, fullheight * 0.8f)
        .Sequence(true)
        .Follow(0)
        .Parent(nullptr);

    auto background_large =
        UI<BaseBackground>("largebg", 1, img_itemcontain, 3, 3);

    background_large->setNativeScale(20);

    background_large->Configure()
        .Parent(Container.get())
        .Anchor(AnchorPoint::TopCenter)
        .Scale(0.9f, 0.7f)
        .Posite(0.5f, 0.15f);

    Elements->PushElement(std::move(background_large));

    auto settingtitle =
        UI<ImageBoard>("settingtitle", 99, img_settingstitle, 1, 1);

    settingtitle->Configure()
        .Anchor(AnchorPoint::TopCenter)
        .Parent(Container.get())
        .Scale(0.113f, 0.0f)
        .Posite(0.5f, 0.05f);

    Elements->PushElement(std::move(settingtitle));

    auto scroll = UI<Scrollbar>("scroll", 3, 0, 0, 0);

    scroll->Configure()
        .Anchor(AnchorPoint::Center)
        .Parent(Container.get())
        .Scale(0.7f, 0.033f)
        .Posite(0.5f, 0.3f);

    scroll->bindVariable(OpenCoreManagers::SetManager.getMusicVolume());

    Elements->PushElement(std::move(scroll));

    auto buttonBorder = UI<ImageBoard>("buttonBorders", 3, button_border, 1, 1);
    buttonBorder->Configure()
        .Parent(Container.get())
        .Scale(0.06f, 0.0f)
        .Posite(0.92f, 0.10f)
        .Anchor(AnchorPoint::Center)
        .Sequence(false);

    Elements->PushElement(std::move(buttonBorder));

    // 返回按钮
    auto backButton = UI<Button>("backButton", 3, img_BackButton, 1, 3);
    backButton->Configure()
        .Parent(Container.get())
        .Scale(0.05f, 0.0f)
        .Posite(0.92f, 0.10f)
        .Anchor(AnchorPoint::Center)
        .Sequence(false);

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

    // 将背景推送
    Elements->PushElement(std::move(Container));
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

void SettingsStage::onExit()
{
    OpenCoreManagers::SetManager.RefreshSettings();
    sController->removeStage(this->stageType);
}

bool SettingsStage::handlEvents(SDL_Event *event)
{
    Elements->handlEvents(*event, timer->getTotalTime());
    return true;
}

void SettingsStage::onDestroy() {}

void SettingsStage::onRender() { Elements->onRender(); }