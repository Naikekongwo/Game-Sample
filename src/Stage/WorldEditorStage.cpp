#include "Eclipsea/Core/AudioManager.hpp"
#include "Eclipsea/Core/EclipseaTextures.hpp"
#include "Eclipsea/Eclipsea.hpp"
#include "Eclipsea/Gameplay/Gameplay.hpp"
#include "Eclipsea/Gameplay/UI/MapExplorer.hpp"
#include "Eclipsea/Stage/PurifierStage.hpp"
#include "OpenCore.hpp"
#include <memory>

// WorldEditorStage（新 OpenCore）：timer / sController 由 StageManager 自动注入
WorldEditorStage::WorldEditorStage()
{
    // BaseStage 基类已设置 stageType 与 Elements
}

void WorldEditorStage::onEnter() { initializeComponents(); }

void WorldEditorStage::Init()
{
    auto &SFX = Eclipsea::AudioManager::getInstance();
    SFX.stopBGM();

    unique_ptr<ImageBoard> background = std::make_unique<ImageBoard>(
        "background", 0, MakeTexture(1, 1, "background_gameplay"));

    background->Configure()
        .Anchor(AnchorPoint::Center)
        .PositeR(0.5f, 0.5f)
        .ScaleR(1.0f, 1.0f);

    Elements->PushElement(std::move(background));

    LOG("尝试添加世界编辑器地图！");

    sController->removeStage(StageType::overlayStage);

    auto mapexp = std::make_unique<MapExplorer>("map", 3);

    mapexp->Configure()
        .Anchor(AnchorPoint::Center)
        .PositeR(0.5f, 0.5f)
        .ScaleR(0.9f, 0.9f)
        .Alpha(1.0f);

    mapexp->setWorldController(&getServerWorldController());
    mapexp->onEnter();
    mapexp->setExplorerViewPort(ViewportType::Fullscreen);

    Elements->PushElement(std::move(mapexp));

    auto gameplay = std::make_unique<PurifierStage>();
    sController->changeStage(std::move(gameplay));
}

void WorldEditorStage::onUpdate()
{
    getServerWorldController().onUpdate(timer->getTotalTime());
    Elements->onUpdate(timer->getTotalTime());
}

void WorldEditorStage::initializeComponents() { Init(); }

void WorldEditorStage::onRender() { Elements->onRender(); }

bool WorldEditorStage::parseEvents(Event *event)
{
    Elements->parseEvents(event, timer->getTotalTime());
    return true;
}
