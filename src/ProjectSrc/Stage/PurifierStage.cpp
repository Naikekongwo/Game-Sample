#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"

PurifierStage::PurifierStage(Timer *timer, StageController *sController)
{
    this->timer = timer;
    this->sController = sController;

    Elements = std::make_unique<ElementManager>();

    // 声明元素
    stageType = StageType::overlayStage;
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
    auto stageBg = UI<BaseBackground>("purSbg", 0, background_purifier, 3, 3);
    stageBg->setNativeScale(60);
    stageBg->Configure()
        .Scale(0.82f, 0.5f)
        .Anchor(AnchorPoint::Center)
        .Parent(nullptr)
        .Posite(0.5f, 0.28f)
        .Sequence(false);
    
    stageBg->setBakedTexture(true);
    

    Elements->PushElement(std::move(stageBg));

    auto backButton = UI<Button>("backButton", 1, img_BackButton, 1, 3);

    backButton->Configure().Scale(0.03f, 0.03f).Posite(0.88f, 0.05f).Anchor(AnchorPoint::Center).Parent(nullptr);

    

    std::function<void()> destroy = [this]() { 
        SDL_Log("StageController::removeStage called, type=%d", (int)stageType);
        sController->removeStage(this->stageType);
    };
    
    backButton->setOnClick(destroy);

    Elements->PushElement(std::move(backButton));

    return true;
}