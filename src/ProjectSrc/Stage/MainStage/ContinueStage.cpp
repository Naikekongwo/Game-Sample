#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"
#include <memory>

ContinueStage::ContinueStage(Timer *timer, StageManager *sController)
{
    this->timer = timer;
    this->sController = sController;

    Elements = std::make_unique<ElementManager>();

    this->stageType = StageType::topStage;
}

void ContinueStage::onEnter()
{
    // 初始化设置页面，首先创建baseBackground
    auto baseBG = UI<BaseBackground>("set_background", 0, background_purifier,
                                     NULL, NULL);

    baseBG->setNativeScale(60);

    baseBG->Configure()
        .Anchor(AnchorPoint::Center)
        .Posite(0.5f, 0.5f)
        .Scale(0.5f, 0.8f)
        .Sequence(true)
        .Follow(0)
        .Parent(nullptr);

    Elements->PushElement(std::move(baseBG));

    // 返回按钮
    auto backButton = UI<Button>("backButton", 1, img_BackButton, 1, 3);
    backButton->Configure()
        .Scale(0.03f, 0.03f)
        .Posite(0.72f, 0.08f)
        .Anchor(AnchorPoint::Center)
        .Sequence(false)
        .Parent(nullptr);

    // 点击回调
    backButton->setOnClick(
        [this]()
        {
            if (phase != ContinuePhase::Ready)
                return; // 防止多次点击
            phase = ContinuePhase::Exiting;
        });
    Elements->PushElement(std::move(backButton));
    // 将元素添加进入
}

void ContinueStage::onUpdate()
{
    Elements->onUpdate(timer->getTotalTime());
    // 刷新函数
    switch (phase)
    {
    case ContinuePhase::Entering:
    {
        auto bgc = Elements->find("set_background");
        if (bgc && bgc->isAnimeFinished())
            phase = ContinuePhase::Ready; // 准备完成的标志
        break;
    }
    case ContinuePhase::Ready:
        break;
    case ContinuePhase::Exiting:
        onExit();
        break;
    default:
        break;
    }
}

void ContinueStage::onExit() { sController->removeStage(this->stageType); }

bool ContinueStage::handlEvents(SDL_Event *event)
{
    Elements->handlEvents(*event, timer->getTotalTime());
    return true;
}

void ContinueStage::onDestroy() {}

void ContinueStage::onRender() { Elements->onRender(); }

void ContinueStage::initializeComponents() {}

bool ContinueStage::parseEvents(Event *event) { return true; }