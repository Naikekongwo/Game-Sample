#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"

GameplayStage::GameplayStage(Timer *timer, StageManager *sController)
{
    this->timer = timer;

    Elements = std::make_unique<ElementManager>();

    this->sController = sController;

    this->stageType = StageType::baseStage;

    Init();
}

void GameplayStage::Init()
{
    auto &SFX = OpenCoreManagers::SFXManager;

    SFX.stopBGM();

    std::unique_ptr<ImageBoard> background = std::make_unique<ImageBoard>(
        "background", 0, MakeTexture(1, 1, background_gameplay));

    background->Configure()
        .Anchor(AnchorPoint::Center)
        .Posite(0.5 * fullwidth, 0.5 * fullheight)
        .Scale(fullwidth, fullheight);

    Elements->PushElement(std::move(background));

    SDL_Log("尝试添加！");

    sController->removeStage(StageType::overlayStage);

    auto waste = std::make_unique<ImageBoard>("wasteland", 1,
                                              MakeTexture(1, 1, wasteland));

    waste->Configure()
        .Anchor(AnchorPoint::Center)
        .Posite(0.5 * fullwidth, 0.5 * fullheight)
        .Scale(fullwidth * 0.1, fullwidth * 0.1);

    Elements->PushElement(std::move(waste));

    auto purifier = std::make_unique<PurifierStage>(timer, sController);

    sController->changeStage(std::move(purifier));
}

void GameplayStage::onUpdate() { Elements->onUpdate(timer->getTotalTime()); }

void GameplayStage::onRender() { Elements->onRender(); }

bool GameplayStage::handlEvents(SDL_Event *event)
{
    return true;
    // 暂时没有动作
}