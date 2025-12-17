#include "OpenCore/OpenCore.hpp"
#include "Eclipsea/Eclipsea.hpp"

GameplayStage::GameplayStage(Timer* timer, StageController* sController)
{
    this->timer = timer;

    Elements = std::make_unique<ElementManager>();
    
    this->sController = sController;

    this->stageType = StageType::baseStage;

    Init();
}

void GameplayStage::Init()
{
    auto& SFX = OpenCoreManagers::SFXManager;

    SFX.stopBGM();

    std::unique_ptr<ImageBoard> background = std::make_unique<ImageBoard>("background", 0, MakeTexture(1,1,background_gameplay));
    // std::unique_ptr<ImageBoard> connector = std::make_unique<ImageBoard>("connector", 99, MakeTexture(1,1,img_connector));
    // // 创建了遮罩和背景的资源

    background->Configure().Anchor(AnchorPoint::Center).Posite(0.5 * fullwidth, 0.5 * fullheight).Scale(fullwidth,fullheight);

    // connector->Configure().Scale(1920,1080).Anchor(AnchorPoint::TopLeft).Posite(0,0);
    // connector->Animate().Move(0,0,1920,0,5.0f, false).Commit();

    Elements->PushElement(std::move(background));
    // Elements->PushElement(std::move(connector));

    SDL_Log("尝试添加！");

    // 临时加入
    auto purifier = std::make_unique<PurifierStage>(timer, sController);

    sController->changeStage(std::move(purifier));

}



void GameplayStage::onUpdate()
{
    Elements->onUpdate(timer->getTotalTime());
}

void GameplayStage::onRender()
{
    Elements->onRender();
}

bool GameplayStage::handlEvents(SDL_Event* event)
{
    return true;
    // 暂时没有动作
}