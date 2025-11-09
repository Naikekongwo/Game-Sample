#include "OpenCore/OpenCore.hpp"
#include "Eclipsea/Eclipsea.hpp"

MainStage::MainStage(Timer* timer, StageController* sController)
{
    this->timer = timer;

    Elements = std::make_unique<ElementManager>();

    this->stageType = StageType::baseStage;

    Init();
}

void MainStage::Init()
{
    auto background = UI<ImageBoard>("background", 0, 2006, 1, 1);
    auto button1    = UI<Button>("StartButton", 1, 2008, 1, 3);
    auto connector  = UI<ImageBoard>("connector", 99, 2005, 1, 1);
    auto stageBg    = UI<BaseBackground>("BaseBackground", 0, 2009, 3, 3);

    auto itemBox = UI<ImageBoard>("item", 1, 2013, 1, 2);

    stageBg->setBakedTexture(true);
    // 创建了遮罩和背景的资源

    background->Configure().Anchor(AnchorPoint::Center).Posite(960, 540).Scale(1920,1080);
    button1->Configure().Anchor(AnchorPoint::Center).Posite(960,540).Scale(512,256).Parent(stageBg.get());
    itemBox->Configure().Anchor(AnchorPoint::Center).Posite(960,800).Scale(128,128);

    connector->Configure().Scale(1920,1080).Anchor(AnchorPoint::TopLeft).Posite(0,0);
    connector->Animate().Move(0,0,1920,0,5.0f, false).Commit();

    stageBg->Configure().Anchor(AnchorPoint::Center).Posite(960,540).Scale(1600,960);

    stageBg->setNativeScale(60);

    // 获取原始指针用于回调
    ImageBoard* itemBoxRaw = itemBox.get();

    button1->setOnClick([itemBoxRaw]() { itemBoxRaw->Animate().Frame(2,2,false).Commit();});


    Elements->PushElement(std::move(background));
    Elements->PushElement(std::move(stageBg));
    Elements->PushElement(std::move(button1));
    Elements->PushElement(std::move(connector));
    Elements->PushElement(std::move(itemBox));

    // auto bottle_full = std::make_shared<Item>(1, "水壶_满", std::move(std::make_unique<Texture>(1,1, OpenCoreManagers::ResManager.GetTexture(2011))));
    // auto bottle_empty = std::make_shared<Item>(2, "水壶_空", std::move(std::make_unique<Texture>(1,1, OpenCoreManagers::ResManager.GetTexture(2012))));

    // auto& ItemMgr = Gameplay::ItemMgr.getInstance();

    // ItemMgr.registerItem(bottle_full);
    // ItemMgr.registerItem(bottle_empty);
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