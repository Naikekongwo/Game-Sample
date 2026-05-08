#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Animation/IAnimation.hpp"
#include "OpenCore/Runtime/Graphics/UI/TextArea.hpp"
#include <cstddef>
#include <memory>

ContinueStage::ContinueStage(Timer *timer, StageManager *sController)
{
    this->timer = timer;
    this->sController = sController;

    Elements = std::make_unique<ElementManager>();

    this->stageType = StageType::topStage;
}

void ContinueStage::onEnter() { initializeComponents(); }

void ContinueStage::onUpdate()
{
    Elements->onUpdate(timer->getTotalTime());
    // 刷新函数
    switch (phase)
    {
    case ContinuePhase::Entering:
    {
        auto bgc = Elements->find("set_background");
        if (bgc && bgc->isAnimeFinished())
            phase = ContinuePhase::Ready; // 准备完成的标志
        break;
    }
    case ContinuePhase::Ready:
        break;
    case ContinuePhase::Exiting:
        onExit();
        break;
    default:
        break;
    }
}

void ContinueStage::onExit() { sController->removeStage(this->stageType); }

bool ContinueStage::handlEvents(SDL_Event *event)
{
    Elements->handlEvents(*event, timer->getTotalTime());
    return true;
}

void ContinueStage::onDestroy() {}

void ContinueStage::onRender() { Elements->onRender(); }

bool ContinueStage::parseEvents(Event *event) { return true; }

void ContinueStage::initializeComponents()
{
    // 初始化设置页面，首先创建窗体的轮廓
    auto Container =
        UI<BaseBackground>("set_background", 0, stone_background, NULL, NULL);
    Container->setNativeScale(128);

    Container->Configure()
        .Anchor(AnchorPoint::Center)
        .Posite(0.5f, 0.5f)
        .Scale(0.6f, 0.8f)
        .Sequence(true)
        .Follow(0)
        .Parent(nullptr);

    // 左侧地图的容器
    auto MapInfoDiv =
        UI<BaseBackground>("mapInfoDiv", 1, img_itemcontain, NULL, NULL);

    MapInfoDiv->setNativeScale(20);

    MapInfoDiv->Configure()
        .Parent(Container.get())
        .Anchor(AnchorPoint::TopRight)
        .Scale(0.3f, 0.75f)
        .Posite(0.35f, 0.15f);

    Elements->PushElement(std::move(MapInfoDiv));

    // 右侧地图信息的容器
    auto InfoDiv =
        UI<BaseBackground>("infoDiv", 1, img_itemcontain, NULL, NULL);

    InfoDiv->setNativeScale(20);

    InfoDiv->Configure()
        .Parent(Container.get())
        .Anchor(AnchorPoint::TopLeft)
        .Scale(0.58f, 0.75f)
        .Posite(0.37f, 0.15f);

    Elements->PushElement(std::move(InfoDiv));

    auto textDemo = UI<TextArea>("textDemo", 50, 9001, NULL, NULL);

    textDemo->Configure()
        .Parent(nullptr)
        .Anchor(AnchorPoint::Center)
        .Alpha(1.0f)
        .Scale(1.0f, 0.2f)
        .Posite(0.5f, 0.5f);

    textDemo->setText("Super Huge Dick");
    textDemo->setShadow(true, 20);

    Elements->PushElement(std::move(textDemo));

    // 标题
    auto title = UI<TextArea>("title", 99, 9002, NULL, NULL);

    title->Configure()
        .Anchor(AnchorPoint::TopCenter)
        .Parent(Container.get())
        .Scale(0.226f, 0.0717f)
        .Posite(0.5f, 0.05f);

    title->setText("本地存档");
    title->setFontSize(62);
    title->setShadow(true, 2);

    Elements->PushElement(std::move(title));

    auto buttonBorder = UI<ImageBoard>("buttonBorders", 3, button_border, 1, 1);
    buttonBorder->Configure()
        .Parent(Container.get())
        .Scale(0.06f, 0.0f)
        .Posite(0.92f, 0.08f)
        .Anchor(AnchorPoint::Center)
        .Sequence(false);

    Elements->PushElement(std::move(buttonBorder));

    // 返回按钮
    auto backButton = UI<Button>("backButton", 3, img_BackButton, 1, 3);
    backButton->Configure()
        .Parent(Container.get())
        .Scale(0.05f, 0.0f)
        .Posite(0.92f, 0.08f)
        .Anchor(AnchorPoint::Center)
        .Sequence(false);

    // 点击回调
    backButton->setOnClick(
        [this]()
        {
            if (phase != ContinuePhase::Ready)
                return; // 防止多次点击
            phase = ContinuePhase::Exiting;
        });
    Elements->PushElement(std::move(backButton));
    // 将元素添加进入

    // 将背景推送
    Elements->PushElement(std::move(Container));
}