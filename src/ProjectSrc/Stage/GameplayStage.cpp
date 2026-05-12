#include "Eclipsea/Eclipsea.hpp"
#include "Eclipsea/Stage/PurifierStage.hpp"
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Animation/IAnimation.hpp"
#include "OpenCore/Runtime/Graphics/UI/MapExplorer.hpp"
#include "OpenCore/Runtime/Graphics/UI/TypeWriter.hpp"
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

    auto tpwt = UI<TypeWriter>("typeWriter", 99, 9002, NULL, NULL);

    tpwt->Configure()
        .Parent(nullptr)
        .Anchor(AnchorPoint::TopCenter)
        .Posite(0.5f, 0.5f)
        .Scale(1.0f, 0.5f);

    tpwt->setFontSize(40);
    tpwt->setText("公元 2300年 "
                  "X市\n地球水资源危机已经进入终末期，先前对生态恢复抱有幻想的"
                  "科学家十不存一，甚至极大多数已经登上了早些时候的诺亚13号飞船"
                  "。\n地球即将进入无人居住的窘态。");
    tpwt->alignCenter(true);

    Elements->PushElement(std::move(tpwt));
}

void GameplayStage::onUpdate() { Elements->onUpdate(timer->getTotalTime()); }

void GameplayStage::onRender() { Elements->onRender(); }

bool GameplayStage::handlEvents(SDL_Event *event)
{
    Elements->handlEvents(*event, timer->getTotalTime());
    return true;
}

void GameplayStage::initializeComponents() {}

bool GameplayStage::parseEvents(Event *event) { return true; }