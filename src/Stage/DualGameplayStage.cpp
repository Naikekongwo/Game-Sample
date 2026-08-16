#include "Eclipsea/Gameplay/Gameplay.hpp"
#include "Eclipsea/Core/EclipseaTextures.hpp"
#include "Eclipsea/Eclipsea.hpp"
#include "Eclipsea/Stage/PurifierStage.hpp"
#include "OpenCore.hpp"
#include "Eclipsea/Gameplay/UI/MapExplorer.hpp"
#include <SDL3/SDL.h>
#include <memory>
#include "Eclipsea/Core/AudioManager.hpp"

// DualGameplayStage（新 OpenCore）：timer / sController 由 StageManager 自动注入
DualGameplayStage::DualGameplayStage()
{
    // BaseStage 基类已设置 stageType 与 Elements
}

void DualGameplayStage::onEnter() { initializeComponents(); }

void DualGameplayStage::Init()
{
    auto &SFX = Eclipsea::AudioManager::getInstance();
    SFX.stopBGM();

    unique_ptr<ImageBoard> background = std::make_unique<ImageBoard>(
        "background", 0, MakeTexture(1, 1, "background_gameplay"));

    background->Configure()
        .Anchor(AnchorPoint::Center)
        .Posite(0.5f, 0.5f)
        .Scale(1.0f, 1.0f);

    Elements->PushElement(std::move(background));

    LOG("尝试添加双人地图！");

    sController->removeStage(StageType::overlayStage);

    auto mapexp = std::make_unique<MapExplorer>("map", 3);

    mapexp->Configure()
        .Anchor(AnchorPoint::Center)
        .Posite(0.25f, 0.5f)
        .Scale(0.5f, 1.0f)
        .Alpha(1.0f);

    mapexp->setWorldController(&getServerWorldController());
    mapexp->onEnter();
    mapexp->setExplorerViewPort(ViewportType::LeftHalf);

    auto mapexp2 = std::make_unique<MapExplorer>("map2", 3);

    mapexp2->Configure()
        .Anchor(AnchorPoint::Center)
        .Posite(0.75f, 0.5f)
        .Scale(0.5f, 1.0f)
        .Alpha(1.0f);

    mapexp2->setWorldController(&getServerWorldController());
    mapexp2->onEnter();
    mapexp2->setExplorerViewPort(ViewportType::RightHalf);
    mapexp2->setIndex(2);

    Elements->PushElement(std::move(mapexp));
    Elements->PushElement(std::move(mapexp2));
}

void DualGameplayStage::onUpdate()
{
    Elements->onUpdate(timer->getTotalTime());
}

void DualGameplayStage::onRender() { Elements->onRender(); }

void DualGameplayStage::initializeComponents() { Init(); }

bool DualGameplayStage::parseEvents(Event *event)
{
    Elements->parseEvents(event, timer->getTotalTime());
    return true;
}
