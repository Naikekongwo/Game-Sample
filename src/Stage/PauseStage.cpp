#include "Eclipsea/Eclipsea.hpp"
#include "Eclipsea/Stage/MainStage.hpp"
#include "OpenCore.hpp"
#include "Runtime/Animation/IAnimation.hpp"
#include "Runtime/Graphics/IDrawableObject/Text.hpp"
#include "Runtime/Graphics/UI/TextButton.hpp"
#include "World/Stage/OverlayStage.hpp"
#include <cstdlib>
#include <stdexcept>

namespace
{
// 暂停界面文字按钮渲染选项：字体本身白色、无描边、有阴影、无渐变（三态共用）
inline constexpr TextRenderOption kPauseTextOption =
    static_cast<TextRenderOption>(RENDER_TEXT | RENDER_SHADOW);

// 暂停界面文字按钮三态属性（与主菜单风格一致）：渲染选项一致，
// 仅文字颜色区分状态反馈。字号由 TextButton 按按钮高度自适应。
inline const TextAttribute kPauseTextNormal{
    .option         = kPauseTextOption,
    .color          = White, // 字体本身白色
    .fontName       = "ChineseFont",
    .borderSize     = 0,
    .shadowGradient = true,
    .shadowOffset   = {2, 2},
};
inline const TextAttribute kPauseTextHovered{
    .option         = kPauseTextOption,
    .color          = Color(0.75f, 0.75f, 0.75f, 1.0f), // 悬浮：整体变灰
    .fontName       = "ChineseFont",
    .borderSize     = 0,
    .shadowGradient = true,
    .shadowOffset   = {2, 2},
};
inline const TextAttribute kPauseTextPressed{
    .option         = kPauseTextOption,
    .color          = Color(0.45f, 0.45f, 0.45f, 1.0f), // 按下：颜色加深
    .fontName       = "ChineseFont",
    .borderSize     = 0,
    .shadowGradient = true,
    .shadowOffset   = {2, 2},
};
} // namespace

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
    auto Container =
        UI<BaseBackground>("set_background", 0, "stone_background", 0, 0);
    Container->setNativeScale(128);

    Container->Configure()
        .Anchor(AnchorPoint::Center)
        .PositeR(0.5f, 0.5f)
        .ScaleR(0.15f, 0.3f)
        .Sequence(true)
        .Follow(0)
        .Parent(nullptr);

    auto title = UI<TextArea>("settingtitle", 99, "9002", 0, 0);

    title->Configure()
        .Anchor(AnchorPoint::TopCenter)
        .Parent(nullptr)
        .ScaleR(0.200f, 0.06f)
        .PositeR(0.5f, 0.24f);

    title->setText("游戏暂停");
    title->setFontSize(50);
    title->setShadow(true, 2);
    title->alignCenter(true);

    Elements->PushElement(std::move(title));

    auto backmain = UI<TextButton>("btn_backmain", 2, "返回主菜单", 0, 0);
    backmain->setNormalAttribute(kPauseTextNormal);
    backmain->setHoveredAttribute(kPauseTextHovered);
    backmain->setPressedAttribute(kPauseTextPressed);
    backmain->align(AnchorPoint::Center); // 文字居中

    backmain->Configure()
        .Parent(Container.get())
        .Anchor(AnchorPoint::Center)
        .PositeR(0.5f, 0.3f)
        .ScaleR(0.52f, 0.123f); // 与 MainStage 按钮一致（约 599×160）

    backmain->setOnClick(
        [this]()
        {
            LOG("返回主菜单");
            auto main = std::make_unique<MainStage>();
            sController->changeStage(std::move(main));
        });

    Elements->PushElement(std::move(backmain));

    auto btn_setting = UI<TextButton>("btn_setting", 2, "设置", 0, 0);
    btn_setting->setNormalAttribute(kPauseTextNormal);
    btn_setting->setHoveredAttribute(kPauseTextHovered);
    btn_setting->setPressedAttribute(kPauseTextPressed);
    btn_setting->align(AnchorPoint::Center); // 文字居中

    btn_setting->Configure()
        .Parent(Container.get())
        .Anchor(AnchorPoint::Center)
        .PositeR(0.5f, 0.6f)
        .ScaleR(0.52f, 0.123f); // 与 MainStage 按钮一致（约 599×160）

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

void PauseStage::onUpdate() { Elements->onUpdate(timer->getTotalTime()); }

void PauseStage::onRender() { Elements->onRender(); }

void PauseStage::onExit()
{
    Elements->onDestroy();
    LOG("PauseStage: onExit - cleared elements");
}

void PauseStage::onDestroy() { LOG("PauseStage: onDestroy"); }
