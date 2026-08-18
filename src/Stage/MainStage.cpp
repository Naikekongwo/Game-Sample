#include "Eclipsea/Core/AudioManager.hpp"
#include "Eclipsea/Core/GameSettings.hpp"
#include "Eclipsea/Eclipsea.hpp"
#include "Eclipsea/Stage/MainStage/ContinueStage.hpp"
#include "Eclipsea/Stage/WorldEditorStage.hpp"
#include "OpenCore.hpp"
#include "Runtime/Animation/IAnimation.hpp"
#include "Runtime/Graphics/IDrawableObject/Text.hpp"
#include "Runtime/Graphics/UI/TextButton.hpp"
#include <SDL3/SDL.h>
#include <functional>
#include <memory>

namespace
{
// 主菜单文字按钮渲染选项：字体本身白色、无描边、有阴影、无渐变（三态共用）
inline constexpr TextRenderOption kMainMenuTextOption =
    static_cast<TextRenderOption>(RENDER_TEXT | RENDER_SHADOW);

// 主菜单文字按钮三态属性：渲染选项一致，仅文字颜色区分状态反馈。
// 字号不在此硬编码——TextButton 会按按钮实际高度自适应（0.9 × 高度）。
inline const TextAttribute kMainMenuTextNormal{
    .option         = kMainMenuTextOption,
    .color          = White, // 字体本身白色
    .fontName       = "ChineseFont",
    .borderSize     = 0,
    .shadowGradient = true,
    .shadowOffset   = {2, 2},
};
inline const TextAttribute kMainMenuTextHovered{
    .option         = kMainMenuTextOption,
    .color          = Color(0.75f, 0.75f, 0.75f, 1.0f), // 悬浮：整体变灰
    .fontName       = "ChineseFont",
    .borderSize     = 0,
    .shadowGradient = true,
    .shadowOffset   = {2, 2},
};
inline const TextAttribute kMainMenuTextPressed{
    .option         = kMainMenuTextOption,
    .color          = Color(0.45f, 0.45f, 0.45f, 1.0f), // 按下：颜色加深
    .fontName       = "ChineseFont",
    .borderSize     = 0,
    .shadowGradient = true,
    .shadowOffset   = {2, 2},
};
} // namespace

// MainStage（新 OpenCore）：timer / sController 由 StageManager 自动注入
MainStage::MainStage()
{
    // OverlayStage 基类已设置 stageType 与 Elements
}

void MainStage::onEnter()
{
    Eclipsea::AudioManager::getInstance().stopBGM();
    phase = MainStagePhase::Idle;
    initializeComponents();
}

void MainStage::initializeComponents()
{
    setupBackground();
    setupTitle();
    setupButtons();
}

void MainStage::setupBackground()
{
    auto bg = UI<ImageBoard>("base_sky", 0, "base_sky", 1, 1);
    bg->Configure()
        .Anchor(AnchorPoint::Center)
        .PositeR(0.5f, 0.5f)
        .ScaleR(1.1f, 0.0f)
        .Follow(30);
    Elements->PushElement(std::move(bg));

    auto bg1 = UI<ImageBoard>("dessert_top", 1, "cities_top", 1, 1);
    bg1->Configure()
        .Anchor(AnchorPoint::Center)
        .PositeR(0.5f, 0.5f)
        .ScaleR(1.2f, 0.0f)
        .Follow(40);
    bg1->Animate()
        .Timer(5.0f)
        .Move(
            static_cast<uint16_t>(
                0.5f * Eclipsea::GameSettings::getInstance().getTargetWidth()),
            static_cast<uint16_t>(
                1.5f * Eclipsea::GameSettings::getInstance().getTargetHeight()),
            static_cast<uint16_t>(
                0.5f * Eclipsea::GameSettings::getInstance().getTargetWidth()),
            static_cast<uint16_t>(
                0.5f * Eclipsea::GameSettings::getInstance().getTargetHeight()),
            5.0f)
        .Commit();
    Elements->PushElement(std::move(bg1));

    auto connector = UI<ImageBoard>("connector", 99, "img_connector", 1, 1);
    connector->Configure()
        .Anchor(AnchorPoint::TopRight)
        .PositeR(0, 0)
        .ScaleR(0.0f, 1.0f);
    connector->Animate()
        .Move(Eclipsea::GameSettings::getInstance().designX(2700), 0,
              Eclipsea::GameSettings::getInstance().designX(6000), 0, 5.0f,
              false)
        .Commit();
    Elements->PushElement(std::move(connector));
}

void MainStage::setupTitle()
{
    auto title = UI<ImageBoard>("mainTitle", 2, "main_title", 1, 1);
    title->Configure()
        .Anchor(AnchorPoint::TopLeft)
        .Parent(nullptr)
        .PositeR(0.08333f, 0.12f)
        .ScaleR(0.365f, 0.13f * widthheight)
        .Sequence(true)
        .Follow(20)
        .Alpha(0.0f);
    title->Animate().Timer(5.0f).Fade(0.0f, 1.0f, 5.0f).Commit();
    Elements->PushElement(std::move(title));

    auto copyright_icon = UI<ImageBoard>("copyrights", 2, "copyright", 1, 1);
    copyright_icon->Configure()
        .Anchor(AnchorPoint::BottomLeft)
        .Parent(nullptr)
        .PositeR(0.0167f, 0.97f)
        .ScaleR(0.258f, 0.0276f * widthheight)
        .Sequence(true)
        .Alpha(0.0f)
        .Follow(20);
    copyright_icon->Animate().Timer(5.0f).Fade(0.0f, 1.0f, 5.0f).Commit();

    Elements->PushElement(std::move(copyright_icon));
}

void MainStage::setupButtons()
{
    // 创建文字按钮（TextButton 实时渲染文字，不依赖图片资源）
    auto startButton    = UI<TextButton>("startButton", 2, "开始游戏", 0, 0);
    auto continueButton = UI<TextButton>("continueButton", 2, "继续游戏", 0, 0);
    auto settingButton  = UI<TextButton>("settingButton", 2, "游戏设置", 0, 0);

#ifdef ECLIPSEA_DEBUG_WORLD
    {
        auto worldbutton = UI<TextButton>("worldbutton", 2, "创建世界", 0, 0);
        worldbutton->setNormalAttribute(kMainMenuTextNormal);
        worldbutton->setHoveredAttribute(kMainMenuTextHovered);
        worldbutton->setPressedAttribute(kMainMenuTextPressed);

        worldbutton->Configure()
            .Anchor(AnchorPoint::TopLeft)
            .Parent(nullptr)
            .PositeR(0.72f, 0.58f)
            .ScaleR(0.156f, 0.0416f * widthheight)
            .Sequence(true);

        worldbutton->setOnClick(
            [this]()
            {
                auto worlde = std::make_unique<WorldEditorStage>();
                sController->changeStage(std::move(worlde));
            });

        worldbutton->align(AnchorPoint::MiddleLeft);

        Elements->PushElement(std::move(worldbutton));
    }
#endif

    // 三态文字属性：渲染选项一致，颜色区分悬浮/按下反馈
    startButton->setNormalAttribute(kMainMenuTextNormal);
    startButton->setHoveredAttribute(kMainMenuTextHovered);
    startButton->setPressedAttribute(kMainMenuTextPressed);

    continueButton->setNormalAttribute(kMainMenuTextNormal);
    continueButton->setHoveredAttribute(kMainMenuTextHovered);
    continueButton->setPressedAttribute(kMainMenuTextPressed);

    settingButton->setNormalAttribute(kMainMenuTextNormal);
    settingButton->setHoveredAttribute(kMainMenuTextHovered);
    settingButton->setPressedAttribute(kMainMenuTextPressed);

    // 配置位置与缩放（高度与位置与原来一致）
    startButton->Configure()
        .Anchor(AnchorPoint::TopLeft)
        .Parent(nullptr)
        .PositeR(0.755f, 0.68f)
        .ScaleR(0.156f, 0.0416f * widthheight)
        .Sequence(true);

    continueButton->Configure()
        .Anchor(AnchorPoint::TopLeft)
        .Parent(nullptr)
        .PositeR(0.79f, 0.78f)
        .ScaleR(0.156f, 0.0416f * widthheight)
        .Sequence(true);

    settingButton->Configure()
        .Anchor(AnchorPoint::TopLeft)
        .Parent(nullptr)
        .PositeR(0.825f, 0.88f)
        .ScaleR(0.156f, 0.0416f * widthheight)
        .Sequence(true);

    // 回调
    startButton->setOnClick(
        [this]()
        {
            auto gameplay = std::make_unique<GameplayStage>();
            sController->changeStage(std::move(gameplay));
        });

    continueButton->setOnClick(
        [this]()
        {
            auto story = std::make_unique<ContinueStage>();
            sController->changeStage(std::move(story));
        });

    settingButton->setOnClick(
        [this]()
        {
            LOG("Setting button clicked");
            auto settings = std::make_unique<SettingsStage>();
            sController->changeStage(std::move(settings));
        });

    startButton->align(AnchorPoint::MiddleLeft);
    continueButton->align(AnchorPoint::MiddleLeft);
    settingButton->align(AnchorPoint::MiddleLeft);

    // 推入元素管理器
    Elements->PushElement(std::move(startButton));
    Elements->PushElement(std::move(continueButton));
    Elements->PushElement(std::move(settingButton));
}

void MainStage::onExit()
{
    // 停止动画、音效
}

void MainStage::onUpdate()
{
    Elements->onUpdate(timer->getTotalTime());
    if (phase == MainStagePhase::Idle)
    {
        auto connector = Elements->find("connector");
        if (connector && connector->isAnimeFinished())
        {
            Eclipsea::AudioManager::getInstance().changeBGM("music_lostworld");
            Eclipsea::AudioManager::getInstance().playBGM();
            Elements->removeElement("connector");
        }
    }
}

void MainStage::onRender() { Elements->onRender(); }

bool MainStage::parseEvents(Event *event)
{
    Elements->parseEvents(event, timer->getTotalTime());
    return true;
}
