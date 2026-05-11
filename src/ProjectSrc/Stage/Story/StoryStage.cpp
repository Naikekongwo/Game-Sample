#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Animation/IAnimation.hpp"
#include "OpenCore/Runtime/Graphics/Manager/ElementManager.hpp"
#include "OpenCore/Runtime/Graphics/UI/ImageBoard.hpp"
#include "OpenCore/World/Stage/Stage.hpp"

StoryStage::StoryStage(Timer *timer, StageManager *sController)
{
    this->timer = timer;
    this->sController = sController;

    this->stageType = overlayStage;

    Elements = std::make_unique<ElementManager>();
}

void StoryStage::onEnter()
{
    if (sStatus > StoryStatus::Loading)
        return;
    initializeComponents();
    sStatus = StoryStatus::Ready;
}

bool StoryStage::handlEvents(SDL_Event *event)
{
    Event new_event = Event(*event);
    return parseEvents(&new_event);
}

bool StoryStage::parseEvents(Event *event) { return true; }

void StoryStage::initializeComponents() { func_intro(); }

void StoryStage::onUpdate() { Elements->onUpdate(timer->getTotalTime()); }

void StoryStage::onRender() { Elements->onRender(); }

#pragma region 剧情部分

void StoryStage::func_intro()
{
    if (sStatus > StoryStatus::Ready)
        return;

    auto background = UI<ImageBoard>("background", 0, 3003, 1, 1);

    background->Configure()
        .Sequence(true)
        .Parent(nullptr)
        .Anchor(AnchorPoint::Center)
        .Posite(0.5f, 0.5f)
        .Alpha(0.0f)
        .Scale(1.0f, 0.0f);

    background->Animate().Timer(5.0f).Fade(0.0f, 1.0f, 5.0f).Commit();

    Elements->PushElement(std::move(background));

    auto frontpage = UI<ImageBoard>("frontpage", 2, 3002, 1, 1);

    frontpage->Configure()
        .Sequence(true)
        .Parent(nullptr)
        .Anchor(AnchorPoint::BottomCenter)
        .Posite(0.5f, 1.0f)
        .Alpha(0.0f)
        .Scale(1.0f, 0.0f);

    frontpage->Animate().Timer(5.0f).Fade(0.0f, 1.0f, 5.0f).Commit();

    Elements->PushElement(std::move(frontpage));

    auto text = UI<TextArea>("agetext", 99, 9002, NULL, NULL);

    text->Configure()
        .Parent(nullptr)
        .Anchor(AnchorPoint::Center)
        .Scale(0.5f, 0.25f)
        .Posite(0.5f, 0.55f)
        .Sequence(true);

    text->Animate().Fade(0.0f, 1.0f, 5.0f).Commit();

    text->alignCenter(true);
    text->setFontSize(50);
    text->setText("公元 2300年");

    Elements->PushElement(std::move(text));
    sStatus = StoryStatus::Intro;
}

#pragma endregion