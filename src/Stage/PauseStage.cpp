#include "Eclipsea/Eclipsea.hpp"
#include "Eclipsea/Stage/MainStage.hpp"
#include "OpenCore.hpp"
#include "Runtime/Animation/IAnimation.hpp"
#include "World/Stage/OverlayStage.hpp"
#include <cstdlib>
#include <stdexcept>

// PauseStage（新 OpenCore）：timer / sController 由 StageManager 自动注入
PauseStage::PauseStage()
{
    // OverlayStage 基类已设置 stageType 与 Elements
}

bool PauseStage::parseEvents(Event *event)
{
    Elements->parseEvents(event, timer->getTotalTime());
    return true;
}

void PauseStage::onEnter() { initializeComponents(); }

void PauseStage::initializeComponents()
{
    auto Container = UI<BaseBackground>("set_background", 0,
                                        "stone_background", 0, 0);
    Container->setNativeScale(128);

    Container->Configure()
        .Anchor(AnchorPoint::Center)
        .Posite(0.5f, 0.5f)
        .Scale(0.3f, 0.6f)
        .Sequence(true)
        .Follow(0)
        .Parent(nullptr);

    auto title = UI<TextArea>("settingtitle", 99, "9002", 0, 0);

    title->Configure()
        .Anchor(AnchorPoint::TopCenter)
        .Parent(nullptr)
        .Scale(0.200f, 0.0717f)
        .Posite(0.5f, 0.12f);

    title->setText("游戏暂停");
    title->setFontSize(50);
    title->setShadow(true, 2);
    title->alignCenter(true);

    Elements->PushElement(std::move(title));

    auto backmain = UI<Button>("btn_backmain", 2, "btn_backmain", 1, 3);

    backmain->Configure()
        .Parent(Container.get())
        .Anchor(AnchorPoint::Center)
        .Posite(0.5f, 0.3f)
        .Scale(0.0f, 0.25f);

    backmain->setOnClick(
        [this]()
        {
            LOG("返回主菜单");
            auto main = std::make_unique<MainStage>();
            sController->changeStage(std::move(main));
        });

    Elements->PushElement(std::move(backmain));

    auto btn_setting = UI<Button>("btn_setting", 2, "btn_pause_settings", 1, 3);

    btn_setting->Configure()
        .Parent(Container.get())
        .Anchor(AnchorPoint::Center)
        .Posite(0.5f, 0.6f)
        .Scale(0.0f, 0.25f);

    btn_setting->setOnClick(
        [this]()
        {
            LOG("打开设置");
            auto settings = std::make_unique<SettingsStage>();
            sController->changeStage(std::move(settings));
        });

    Elements->PushElement(std::move(btn_setting));

    Elements->PushElement(std::move(Container));
}

void PauseStage::onUpdate()
{
    Elements->onUpdate(timer->getTotalTime());
}

void PauseStage::onRender()
{
    Elements->onRender();
}

void PauseStage::onExit()
{
    Elements->onDestroy();
    LOG("PauseStage: onExit - cleared elements");
}

void PauseStage::onDestroy()
{
    LOG("PauseStage: onDestroy");
}
