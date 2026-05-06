#include "Eclipsea/Eclipsea.hpp"
#include "Eclipsea/Stage/PurifierStage.hpp"
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Graphics/UI/MapExplorer.hpp"
#include <memory>

WorldEditorStage::WorldEditorStage(Timer *timer, StageManager *sController)
{
    this->timer = timer;

    Elements = std::make_unique<ElementManager>();

    this->sController = sController;

    this->stageType = StageType::baseStage;

    Init();
}

void WorldEditorStage::Init()
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
        .Posite(0.5f, 0.5f)
        .Scale(1.0f, 1.0f)
        .Alpha(1.0f);

    mapexp->setWorldController(
        OpenEngine::getInstance().getServerWorldController());

    mapexp->onEnter();
    mapexp->setExplorerViewPort(ViewportType::Fullscreen);

    Elements->PushElement(std::move(mapexp));

    auto gameplay = std::make_unique<PurifierStage>(timer, sController);
    sController->changeStage(std::move(gameplay));
}

void WorldEditorStage::onUpdate() { Elements->onUpdate(timer->getTotalTime()); }

void WorldEditorStage::onRender() { Elements->onRender(); }

bool WorldEditorStage::handlEvents(SDL_Event *event)
{
    Elements->handlEvents(*event, timer->getTotalTime());
    return true;
    // 暂时没有动作
}

void WorldEditorStage::initializeComponents() {}

bool WorldEditorStage::parseEvents(Event *event) { return true; }