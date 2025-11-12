#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"
#include <functional>

MainStage::MainStage(Timer *timer, StageController *sController)
{
    this->timer = timer;

    this->sController = sController;

    Elements = std::make_unique<ElementManager>();

    this->stageType = StageType::overlayStage;

    Init();
}

void MainStage::Init()
{
    auto background = UI<ImageBoard>("background", 0, background_main, 1, 1);
    auto connector = UI<ImageBoard>("connector", 99, img_connector, 1, 1);

    background->Configure()
        .Anchor(AnchorPoint::Center)
        .Posite(0.5 * fullwidth, 0.5* fullheight)
        .Scale(fullwidth, fullheight);
    connector->Configure()
        .Scale(fullwidth, fullheight)
        .Anchor(AnchorPoint::TopLeft)
        .Posite(0, 0);
    connector->Animate().Move(0, 0, 1920, 0, 5.0f, false).Commit();

    auto title = UI<ImageBoard>("mainTitle", 1, main_title, 1, 1);

    title->Configure()
        .Anchor(AnchorPoint::TopLeft)
        .Parent(nullptr)
        .Posite(0.08333f, 0.0677f)
        .Scale(0.365f, 0.13f)
        .Sequence(true);

    Elements->PushElement(std::move(title));

    auto startButton = UI<Button>("startButton", 1, img_StartButton, 1, 3);
    auto continueButton = UI<Button>("continueButton", 1, img_ContButton, 1, 3);
    auto settingButton = UI<Button>("settingButton", 1, img_SettButton, 1, 3);

    std::function<void()> startAction = [this]()
    {
        auto gameplay = std::make_unique<GameplayStage>(timer, sController);
        sController->changeStage(std::move(gameplay));
        lifeStatus = died;
    };

    startButton->Configure()
        .Anchor(AnchorPoint::TopLeft)
        .Parent(nullptr)
        .Posite(0.755f, 0.383f)
        .Scale(0.156f, 0.0416f)
        .Sequence(true);
    continueButton->Configure()
        .Anchor(AnchorPoint::TopLeft)
        .Parent(nullptr)
        .Posite(0.79f, 0.4375f)
        .Scale(0.156f, 0.0416f)
        .Sequence(true);
    settingButton->Configure()
        .Anchor(AnchorPoint::TopLeft)
        .Parent(nullptr)
        .Posite(0.825f, 0.492f)
        .Scale(0.156f, 0.0416f)
        .Sequence(true);

    // 配置
    startButton->setOnClick(startAction);

    Elements->PushElement(std::move(startButton));
    Elements->PushElement(std::move(continueButton));
    Elements->PushElement(std::move(settingButton));

    // 获取原始指针用于回调
    // ImageBoard* itemBoxRaw = itemBox.get();

    // button1->setOnClick([itemBoxRaw]() {
    // itemBoxRaw->Animate().Frame(2,2,false).Commit();});

    Elements->PushElement(std::move(background));
    Elements->PushElement(std::move(connector));

    // auto bottle_full = std::make_shared<Item>(1, "水壶_满",
    // std::move(std::make_unique<Texture>(1,1,
    // OpenCoreManagers::ResManager.GetTexture(img_SettButton)))); auto
    // bottle_empty = std::make_shared<Item>(2, "水壶_空",
    // std::move(std::make_unique<Texture>(1,1,
    // OpenCoreManagers::ResManager.GetTexture(main_title))));

    // auto& ItemMgr = Gameplay::ItemMgr.getInstance();

    // ItemMgr.registerItem(bottle_full);
    // ItemMgr.registerItem(bottle_empty);
}

void MainStage::onUpdate() { 
    Elements->onUpdate(timer->getTotalTime()); 
}

void MainStage::onRender() { Elements->onRender(); }

bool MainStage::handlEvents(SDL_Event *event)
{
    Elements->handlEvents(*event, timer->getTotalTime());
    return true;
    // 暂时没有动作
}