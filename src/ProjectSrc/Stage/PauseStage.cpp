#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Animation/IAnimation.hpp"
#include "OpenCore/World/Stage/OverlayStage.hpp"
#include <cstdlib>
#include <stdexcept>

PauseStage::PauseStage(Timer *timer, StageManager *sController)
{
    this->timer = timer;
    this->sController = sController;

    Elements = std::make_unique<ElementManager>();

    this->stageType = overlayStage;
}

bool PauseStage::handlEvents(SDL_Event *event)
{
    Event nEvent = Event(*event);
    return parseEvents(&nEvent);
}

bool PauseStage::parseEvents(Event *event)
{
    SDL_Event sEvent = *event;
    Elements->handlEvents(sEvent, timer->getTotalTime());
    return true;
}

void PauseStage::onEnter() { initializeComponents(); }

void PauseStage::initializeComponents()
{
    auto Container =
        UI<BaseBackground>("set_background", 0, stone_background, NULL, NULL);
    Container->setNativeScale(128);

    Container->Configure()
        .Anchor(AnchorPoint::Center)
        .Posite(0.5f, 0.5f)
        .Scale(0.3f, 0.6f)
        .Sequence(true)
        .Follow(0)
        .Parent(nullptr);

    auto title = UI<TextArea>("settingtitle", 99, 9002, NULL, NULL);

    title->Configure()
        .Anchor(AnchorPoint::TopCenter)
        .Parent(nullptr)
        .Scale(0.200f, 0.0717f)
        .Posite(0.5f, 0.12f);

    title->setText("游戏暂停");
    title->setFontSize(50);
    title->setShadow(true, 2);
    title->alignCenter(true);

    Elements->PushElement(std::move(title));

    auto backmain = UI<Button>("btn_backmain", 2, 2048, 1, 3);

    backmain->Configure()
        .Parent(Container.get())
        .Anchor(AnchorPoint::Center)
        .Posite(0.5f, 0.3f)
        .Scale(0.0f, 0.25f);

    Elements->PushElement(std::move(backmain));

    auto btn_setting = UI<Button>("btn_setting", 2, 2049, 1, 3);

    btn_setting->Configure()
        .Parent(Container.get())
        .Anchor(AnchorPoint::Center)
        .Posite(0.5f, 0.6f)
        .Scale(0.0f, 0.25f);

    Elements->PushElement(std::move(btn_setting));

    Elements->PushElement(std::move(Container));
}

void PauseStage::onUpdate() { Elements->onUpdate(timer->getTotalTime()); }

void PauseStage::onRender() { Elements->onRender(); }

void PauseStage::onExit() {}

void PauseStage::onDestroy() {}