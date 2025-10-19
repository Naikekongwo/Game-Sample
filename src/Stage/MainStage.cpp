#include "OpenCore/OpenCore.hpp"

MainStage::MainStage(Timer* timer, StageController* sController)
{
    this->timer = timer;

    Elements = std::make_unique<ElementManager>();

    Init();
}

void MainStage::Init()
{
    auto background = UI<ImageBoard>("background", -1, 2006, 1, 1);
    auto button1    = UI<Button>("StartButton", 1, 2008, 1, 3);
    auto connector  = UI<ImageBoard>("connector", 99, 2005, 1, 1);
    auto stageBg    = UI<StageBackground>("StageBackground", 0, 2009, 3, 3);
    auto frameCounter = UI<FrameCounter>("frameCounter", 100, 0, 0, 0);

    auto itemCollection = UI<ItemContainerCollection>("collection", 60, 2010, 2, 10);

    itemCollection->Configure().Anchor(AnchorPoint::Center).Scale(1568,288).Posite(960, 784);

    stageBg->setBakedTexture(true);
    // 创建了遮罩和背景的资源

    background->Configure().Anchor(AnchorPoint::Center).Posite(960, 540).Scale(1920,1080);
    button1->Configure().Anchor(AnchorPoint::Center).Posite(960,540).Scale(512,256);

    connector->Configure().Scale(1920,1080).Anchor(AnchorPoint::TopLeft).Posite(0,0);
    connector->Animate().Move(0,0,1920,0,5.0f, false).Commit();

    stageBg->Configure().Anchor(AnchorPoint::Center).Posite(960,540).Scale(1600,960);

    stageBg->setNativeScale(60);



    Elements->PushElement(std::move(background));
    Elements->PushElement(std::move(stageBg));
    Elements->PushElement(std::move(button1));
    Elements->PushElement(std::move(connector));
    Elements->PushElement(std::move(frameCounter));
    Elements->PushElement(std::move(itemCollection));
}



void MainStage::onUpdate()
{
    Elements->onUpdate(timer->getTotalTime());
}

void MainStage::onRender()
{
    Elements->onRender();
}

bool MainStage::handlEvents(SDL_Event* event)
{
    Elements->handlEvents(*event, timer->getTotalTime());
    return true;
    // 暂时没有动作
}