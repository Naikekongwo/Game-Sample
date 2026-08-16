#include "Eclipsea/Core/AudioManager.hpp"
#include "Eclipsea/Core/EclipseaTextures.hpp"
#include "Eclipsea/Eclipsea.hpp"
#include "Eclipsea/Gameplay/Gameplay.hpp"
#include "Eclipsea/Gameplay/UI/MapExplorer.hpp"
#include "Eclipsea/Stage/DualGameplayStage.hpp"
#include "Eclipsea/Stage/MainStage.hpp"
#include "Eclipsea/Stage/PauseStage.hpp"
#include "Eclipsea/Stage/PurifierStage.hpp"
#include "OpenCore.hpp"
#include "Runtime/Animation/IAnimation.hpp"
#include "Runtime/Graphics/UI/TypeWriter.hpp"
#include <SDL3/SDL.h>
#include <memory>

// GameplayStage（新 OpenCore）：timer / sController 由 StageManager 自动注入
GameplayStage::GameplayStage()
{
    // BaseStage 基类已设置 stageType 与 Elements
}

void GameplayStage::onUpdate()
{
    // 先驱动世界控制器（实体消费 desiredVelocity 移动），再更新 UI 元素
    getServerWorldController().onUpdate(timer->getTotalTime());
    Elements->onUpdate(timer->getTotalTime());
}

void GameplayStage::onRender() { Elements->onRender(); }

bool GameplayStage::parseEvents(Event *event)
{
    const SDL_Event &sdlEvent = event->GetSDLEvent();
    if (sdlEvent.type == SDL_EVENT_KEY_DOWN)
    {
        switch (sdlEvent.key.key)
        {
        case SDLK_Y:
        {
            auto dualStage = std::make_unique<DualGameplayStage>();
            sController->changeStage(std::move(dualStage));
            break;
        }
        case SDLK_ESCAPE:
        {
            auto pauseStage = std::make_unique<PauseStage>();
            sController->changeStage(std::move(pauseStage));
            break;
        }
        default:
            break;
        }
    }
    Elements->parseEvents(event, timer->getTotalTime());
    return true;
}

void GameplayStage::initializeComponents()
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

    LOG("尝试添加地图！");

    sController->removeStage(StageType::overlayStage);

    auto mapexp = std::make_unique<MapExplorer>("map", 3);

    mapexp->Configure()
        .Anchor(AnchorPoint::Center)
        .PositeR(0.5f, 0.5f)
        .ScaleR(1.0f, 1.0f)
        .Alpha(1.0f);

    mapexp->setWorldController(&getServerWorldController());

    mapexp->onEnter();
    mapexp->setExplorerViewPort(ViewportType::Fullscreen);

    Elements->PushElement(std::move(mapexp));
}
