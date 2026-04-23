#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"
#include <memory>

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

    unique_ptr<ImageBoard> background = std::make_unique<ImageBoard>(
        "background", 0, MakeTexture(1, 1, background_gameplay));

    background->Configure()
        .Anchor(AnchorPoint::Center)
        .Posite(0.5f, 0.5f)
        .Scale(1.0f, 1.0f);

    Elements->PushElement(std::move(background));

    Console_Log("尝试添加！");

    sController->removeStage(StageType::overlayStage);

    auto mapexp = std::make_unique<MapExplorer>("map", 3);

    mapexp->Configure()
        .Anchor(AnchorPoint::Center)
        .Posite(0.5f, 0.5f)
        .Scale(1.0f, 1.0f);

    Elements->PushElement(std::move(mapexp));
}

void GameplayStage::onUpdate() { Elements->onUpdate(timer->getTotalTime()); }

void GameplayStage::onRender() { Elements->onRender(); }

bool GameplayStage::handlEvents(SDL_Event *event)
{
    Elements->handlEvents(*event, timer->getTotalTime());
    return true;
    // 暂时没有动作
}

void GameplayStage::initializeComponents() {}

bool GameplayStage::parseEvents(Event *event) { return true; }