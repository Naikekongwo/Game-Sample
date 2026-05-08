#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Animation/IAnimation.hpp"
#include "OpenCore/Runtime/Graphics/UI/TextArea.hpp"
#include <cstddef>
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
        UI<BaseBackground>("set_background", 0, stone_background, NULL, NULL);
    Container->setNativeScale(128);

    Container->Configure()
        .Anchor(AnchorPoint::Center)
        .Posite(0.5f, 0.5f)
        .Scale(0.6f, 0.8f)
        .Sequence(true)
        .Follow(0)
        .Parent(nullptr);

    auto background_large =
        UI<BaseBackground>("largebg", 1, img_itemcontain, NULL, NULL);

    background_large->setNativeScale(20);

    background_large->Configure()
        .Parent(Container.get())
        .Anchor(AnchorPoint::TopCenter)
        .Scale(0.9f, 0.7f)
        .Posite(0.5f, 0.15f);

    Elements->PushElement(std::move(background_large));

    // auto settingtitle =
    //     UI<ImageBoard>("settingtitle", 99, img_settingstitle, 1, 1);

    // settingtitle->Configure()
    //     .Anchor(AnchorPoint::TopCenter)
    //     .Parent(Container.get())
    //     .Scale(0.113f, 0.0f)
    //     .Posite(0.5f, 0.05f);

    // Elements->PushElement(std::move(settingtitle));

    auto title = UI<TextArea>("settingtitle", 99, 9002, NULL, NULL);

    title->Configure()
        .Anchor(AnchorPoint::TopCenter)
        .Parent(Container.get())
        .Scale(0.113f, 0.0717f)
        .Posite(0.5f, 0.05f);

    title->setText("设置");
    title->setFontSize(62);
    title->setShadow(true, 2);

    Elements->PushElement(std::move(title));

    // 选项名称

    auto musicTitle = UI<TextArea>("musicVol", 3, 9002, NULL, NULL);
    auto soundTitle = UI<TextArea>("soundVol", 3, 9002, NULL, NULL);

    musicTitle->Configure()
        .Parent(Container.get())
        .Anchor(AnchorPoint::TopLeft)
        .Posite(0.14f, 0.22f)
        .Scale(0.5f, 0.0425f);

    musicTitle->setText("音乐音量");
    musicTitle->setFontSize(36);
    musicTitle->setShadow(true, 2);

    soundTitle->Configure()
        .Parent(Container.get())
        .Anchor(AnchorPoint::TopLeft)
        .Posite(0.14f, 0.35f)
        .Scale(0.5f, 0.0425f);

    soundTitle->setText("音效音量");
    soundTitle->setFontSize(36);
    soundTitle->setShadow(true, 2);

    // Elements->PushElement(std::move(musicV));
    Elements->PushElement(std::move(musicTitle));
    Elements->PushElement(std::move(soundTitle));

    auto Multi = UI<TextArea>("multiTitle", 3, 9002, NULL, NULL);

    auto renderTitle = UI<TextArea>("renderTitle", 3, 9002, NULL, NULL);

    renderTitle->Configure()
        .Parent(Container.get())
        .Anchor(AnchorPoint::TopLeft)
        .Posite(0.14f, 0.48f)
        .Scale(0.25f, 0.053f);

    renderTitle->setText("渲染距离");
    renderTitle->setFontSize(36);
    renderTitle->setShadow(true, 2);

    Multi->Configure()
        .Parent(Container.get())
        .Anchor(AnchorPoint::TopLeft)
        .Posite(0.14f, 0.61f)
        .Scale(0.25f, 0.053f);

    Multi->setText("渲染距离");
    Multi->setFontSize(36);
    Multi->setShadow(true, 2);

    Elements->PushElement(std::move(renderTitle));
    Elements->PushElement(std::move(Multi));

    // 选项名称结束

    auto scroll_musicv = UI<Scrollbar>("scroll_musicv", 3, 0, 0, 0);

    scroll_musicv->Configure()
        .Anchor(AnchorPoint::Center)
        .Parent(Container.get())
        .Scale(0.7f, 0.033f)
        .Posite(0.5f, 0.3f);

    scroll_musicv->bindVariable(OpenCoreManagers::SetManager.getMusicVolume());

    Elements->PushElement(std::move(scroll_musicv));

    auto scroll_chunk = UI<Scrollbar>("scroll_chunkv", 3, 0, 0, 0);

    scroll_chunk->Configure()
        .Anchor(AnchorPoint::Center)
        .Parent(Container.get())
        .Scale(0.7f, 0.033f)
        .Posite(0.5f, 0.43f);

    scroll_chunk->bindVariable(OpenCoreManagers::SetManager.getChunkVolume());

    Elements->PushElement(std::move(scroll_chunk));

    auto scroll_renderd = UI<Scrollbar>("scroll_renderd", 3, 0, 0, 0);

    scroll_renderd->Configure()
        .Anchor(AnchorPoint::Center)
        .Parent(Container.get())
        .Scale(0.7f, 0.033f)
        .Posite(0.5f, 0.56f);

    scroll_renderd->bindVariable(
        OpenCoreManagers::SetManager.getRenderFactor());

    Elements->PushElement(std::move(scroll_renderd));

    auto checkbox = UI<CheckBox>("checkbox", 3, img_checkbox, NULL, NULL);

    checkbox->Configure()
        .Parent(Container.get())
        .Anchor(AnchorPoint::TopLeft)
        .Scale(0.0f, 0.053f)
        .Posite(0.27f, 0.61f);

    checkbox->bindVariable(OpenCoreManagers::SetManager.getMultiStatus());
    Elements->PushElement(std::move(checkbox));

    auto buttonBorder = UI<ImageBoard>("buttonBorders", 3, button_border, 1, 1);
    buttonBorder->Configure()
        .Parent(Container.get())
        .Scale(0.06f, 0.0f)
        .Posite(0.92f, 0.08f)
        .Anchor(AnchorPoint::Center)
        .Sequence(false);

    Elements->PushElement(std::move(buttonBorder));

    // 返回按钮
    auto backButton = UI<Button>("backButton", 3, img_BackButton, 1, 3);
    backButton->Configure()
        .Parent(Container.get())
        .Scale(0.05f, 0.0f)
        .Posite(0.92f, 0.08f)
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

bool SettingsStage::parseEvents(Event *event) { return true; }

void SettingsStage::initializeComponents() {}