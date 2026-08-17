#include "Eclipsea/Core/GameSettings.hpp"
#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore.hpp"
#include "Runtime/Animation/IAnimation.hpp"
#include "Runtime/Graphics/UI/TextArea.hpp"
#include <cstddef>
#include <memory>

// ContinueStage（新 OpenCore）：timer / sController 由 StageManager 自动注入
ContinueStage::ContinueStage()
{
    // TopStage 基类已设置 stageType 与 Elements
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
            phase = ContinuePhase::Ready;
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

void ContinueStage::onDestroy() {}

void ContinueStage::onRender() { Elements->onRender(); }

bool ContinueStage::parseEvents(Event *event)
{
    Elements->parseEvents(event, timer->getTotalTime());
    return true;
}

void ContinueStage::initializeComponents()
{
    // 初始化页面，首先创建窗体的轮廓
    auto Container =
        UI<BaseBackground>("set_background", 0, "stone_background", 0, 0);
    Container->setNativeScale(128);

    Container->Configure()
        .Anchor(AnchorPoint::Center)
        .PositeR(0.5f, 0.5f)
        .ScaleR(0.6f, 0.8f)
        .Sequence(true)
        .Follow(0)
        .Parent(nullptr);

    // 左侧地图的容器
    auto MapInfoDiv =
        UI<BaseBackground>("mapInfoDiv", 1, "img_itemcontain", 0, 0);

    MapInfoDiv->setNativeScale(20);

    MapInfoDiv->Configure()
        .Parent(Container.get())
        .Anchor(AnchorPoint::TopRight)
        .ScaleR(0.3f, 0.75f)
        .PositeR(0.35f, 0.15f);

    Elements->PushElement(std::move(MapInfoDiv));

    // 右侧地图信息的容器
    auto InfoDiv = UI<BaseBackground>("infoDiv", 1, "img_itemcontain", 0, 0);

    InfoDiv->setNativeScale(20);

    InfoDiv->Configure()
        .Parent(Container.get())
        .Anchor(AnchorPoint::TopLeft)
        .ScaleR(0.58f, 0.75f)
        .PositeR(0.37f, 0.15f);

    Elements->PushElement(std::move(InfoDiv));

    auto textDemo = UI<TextArea>("textDemo", 50, "9001", 0, 0);

    textDemo->Configure()
        .Parent(nullptr)
        .Anchor(AnchorPoint::Center)
        .Alpha(1.0f)
        .ScaleR(1.0f, 0.2f)
        .PositeR(0.5f, 0.5f);

    textDemo->setText("Super Huge Dick");
    textDemo->setShadow(true, 20);

    Elements->PushElement(std::move(textDemo));

    // 标题
    auto title = UI<TextArea>("title", 99, "9002", 0, 0);

    title->Configure()
        .Anchor(AnchorPoint::TopCenter)
        .Parent(Container.get())
        .ScaleR(0.226f, 0.0717f)
        .PositeR(0.5f, 0.05f);

    title->setText("本地存档");
    title->setFontSize(
        Eclipsea::GameSettings::getInstance().designFontSize(62));
    title->setShadow(true, 2);

    Elements->PushElement(std::move(title));

    auto buttonBorder =
        UI<ImageBoard>("buttonBorders", 3, "button_border", 1, 1);
    buttonBorder->Configure()
        .Parent(Container.get())
        .ScaleR(0.06f, 0.0f)
        .PositeR(0.92f, 0.08f)
        .Anchor(AnchorPoint::Center)
        .Sequence(false);

    Elements->PushElement(std::move(buttonBorder));

    // 返回按钮
    auto backButton = UI<Button>("backButton", 3, "img_BackButton", 1, 3);
    backButton->Configure()
        .Parent(Container.get())
        .ScaleR(0.05f, 0.0f)
        .PositeR(0.92f, 0.08f)
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
    // 将元素添加进去

    // 将背景推入
    Elements->PushElement(std::move(Container));
}
