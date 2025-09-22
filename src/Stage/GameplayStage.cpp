#include "OpenCore/OpenCore.hpp"

GameplayStage::GameplayStage(Timer* timer, StageController* sController)
{
    this->timer = timer;

    Elements = std::make_unique<ElementManager>();

    Init();
}

void GameplayStage::Init()
{
    std::unique_ptr<ImageBoard> background = std::make_unique<ImageBoard>("background", -1,new Texture(1,1,OpenCoreManagers::ResManager.GetTexture(2007)));
    std::unique_ptr<ImageBoard> connector = std::make_unique<ImageBoard>("connector", 99, new Texture(1,1,OpenCoreManagers::ResManager.GetTexture(2005)));
    // 创建了遮罩和背景的资源

    background->Configure().Anchor(AnchorPoint::Center).Posite(960, 540).Scale(1920,1080);

    connector->Configure().Scale(1920,1080).Anchor(AnchorPoint::TopLeft).Posite(0,0);
    connector->Animate().Move(0,0,1920,0,5.0f, false).Commit();

    Elements->PushElement(std::move(background));
    Elements->PushElement(std::move(connector));
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