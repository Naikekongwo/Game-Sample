#include "Eclipsea/Eclipsea.hpp"
#include "Eclipsea/Stage/DualGameplayStage.hpp"
#include "Eclipsea/Stage/PurifierStage.hpp"
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Animation/IAnimation.hpp"
#include "OpenCore/Runtime/Graphics/UI/MapExplorer.hpp"
#include "OpenCore/Runtime/Graphics/UI/TypeWriter.hpp"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
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
}

void GameplayStage::onUpdate() { Elements->onUpdate(timer->getTotalTime()); }

void GameplayStage::onRender() { Elements->onRender(); }

bool GameplayStage::handlEvents(SDL_Event *event)
{
    if (event->type == SDL_KEYDOWN)
    {
        switch (event->key.keysym.sym)
        {
        case SDLK_y:
        {
            auto dualStage =
                std::make_unique<DualGameplayStage>(timer, sController);

            sController->changeStage(std::move(dualStage));

            break;
        }

        default:
            break;
        }
    }
    Elements->handlEvents(*event, timer->getTotalTime());
    return true;
}

void GameplayStage::initializeComponents() {}

bool GameplayStage::parseEvents(Event *event) { return true; }