#include "Eclipsea/Eclipsea.hpp"
#include "Eclipsea/Stage/PurifierStage.hpp"
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Graphics/UI/MapExplorer.hpp"
#include <memory>

DualGameplayStage::DualGameplayStage(Timer *timer, StageManager *sController)
{
    this->timer = timer;

    Elements = std::make_unique<ElementManager>();

    this->sController = sController;

    this->stageType = StageType::baseStage;

    Init();
}

void DualGameplayStage::Init()
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

    LOG("尝试添加！");

    sController->removeStage(StageType::overlayStage);

    auto mapexp = std::make_unique<MapExplorer>("map", 3);

    mapexp->Configure()
        .Anchor(AnchorPoint::Center)
        .Posite(0.25f, 0.5f)
        .Scale(0.5f, 1.0f)
        .Alpha(1.0f);

    mapexp->setWorldController(
        OpenEngine::getInstance().getServerWorldController());

    mapexp->onEnter();
    mapexp->setExplorerViewPort(ViewportType::LeftHalf);

    auto mapexp2 = std::make_unique<MapExplorer>("map2", 3);

    mapexp2->Configure()
        .Anchor(AnchorPoint::Center)
        .Posite(0.75f, 0.5f)
        .Scale(0.5f, 1.0f)
        .Alpha(1.0f);

    mapexp2->setWorldController(
        OpenEngine::getInstance().getServerWorldController());

    mapexp2->onEnter();
    mapexp2->setExplorerViewPort(ViewportType::RightHalf);
    mapexp2->setIndex(2);

    Elements->PushElement(std::move(mapexp));
    Elements->PushElement(std::move(mapexp2));

    auto gameplay = std::make_unique<PurifierStage>(timer, sController);
    sController->changeStage(std::move(gameplay));
}

void DualGameplayStage::onUpdate()
{
    Elements->onUpdate(timer->getTotalTime());
}

void DualGameplayStage::onRender() { Elements->onRender(); }

void DualGameplayStage::initializeComponents() {}

bool DualGameplayStage::parseEvents(Event *event) { return true; }