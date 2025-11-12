#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"

PurifierStage::PurifierStage(Timer *timer, StageController *sController)
{
    this->timer = timer;
    this->sController = sController;

    Elements = std::make_unique<ElementManager>();

    // 声明元素
    stageType = overlayStage;
    constructStage();
}

bool PurifierStage::handlEvents(SDL_Event *event)
{
    Elements->handlEvents(*event, timer->getTotalTime());

    return true;
}

void PurifierStage::onUpdate() { Elements->onUpdate(timer->getTotalTime()); }

void PurifierStage::onRender() { Elements->onRender(); }

bool PurifierStage::constructStage()
{
    // 构建舞台的基本函数
    auto stageBg = UI<BaseBackground>("purSbg", 0, background_purifier, 1, 1);
    stageBg->Configure()
        .Scale(0.8f, 0.45f)
        .Anchor(AnchorPoint::Center)
        .Parent(nullptr)
        .Posite(0.5f, 0.28f)
        .Sequence(false);

    Elements->PushElement(std::move(stageBg));
    return true;
}