#include "Eclipsea/Core/AudioManager.hpp"
#include "Eclipsea/Core/GameSettings.hpp"
#include "Eclipsea/Eclipsea.hpp"
#include "Eclipsea/Stage/MainStage/ContinueStage.hpp"
#include "Eclipsea/Stage/WorldEditorStage.hpp"
#include "OpenCore.hpp"
#include <SDL3/SDL.h>
#include <functional>
#include <memory>


// MainStage（新 OpenCore）：timer / sController 由 StageManager 自动注入
MainStage::MainStage() {
  // OverlayStage 基类已设置 stageType 与 Elements
}

void MainStage::onEnter() {
  Eclipsea::AudioManager::getInstance().stopBGM();
  phase = MainStagePhase::Idle;
  initializeComponents();
}

void MainStage::initializeComponents() {
  setupBackground();
  setupTitle();
  setupButtons();
}

void MainStage::setupBackground() {
  auto bg = UI<ImageBoard>("base_sky", 0, "base_sky", 1, 1);
  bg->Configure()
      .Anchor(AnchorPoint::Center)
      .Posite(0.5f, 0.5f)
      .Scale(1.1f, 0.0f)
      .Follow(30);
  Elements->PushElement(std::move(bg));

  auto bg1 = UI<ImageBoard>("dessert_top", 1, "cities_top", 1, 1);
  bg1->Configure()
      .Anchor(AnchorPoint::Center)
      .Posite(0.5f, 0.5f)
      .Scale(1.2f, 0.0f)
      .Follow(40);
  bg1->Animate()
      .Timer(5.0f)
      .Move(static_cast<uint16_t>(
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
      .Posite(0, 0)
      .Scale(0.0f, 1.0f);
  connector->Animate().Move(2700, 0, 6000, 0, 5.0f, false).Commit();
  Elements->PushElement(std::move(connector));
}

void MainStage::setupTitle() {
  auto title = UI<ImageBoard>("mainTitle", 2, "main_title", 1, 1);
  title->Configure()
      .Anchor(AnchorPoint::TopLeft)
      .Parent(nullptr)
      .Posite(0.08333f, 0.12f)
      .Scale(0.365f, 0.13f * widthheight)
      .Sequence(true)
      .Follow(20)
      .Alpha(0.0f);
  title->Animate().Timer(5.0f).Fade(0.0f, 1.0f, 5.0f).Commit();
  Elements->PushElement(std::move(title));

  auto copyright_icon = UI<ImageBoard>("copyrights", 2, "copyright", 1, 1);
  copyright_icon->Configure()
      .Anchor(AnchorPoint::BottomLeft)
      .Parent(nullptr)
      .Posite(0.0167f, 0.97f)
      .Scale(0.258f, 0.0276f * widthheight)
      .Sequence(true)
      .Alpha(0.0f)
      .Follow(20);
  copyright_icon->Animate().Timer(5.0f).Fade(0.0f, 1.0f, 5.0f).Commit();

  Elements->PushElement(std::move(copyright_icon));
}

void MainStage::setupButtons() {
  // 创建按钮
  auto startButton = UI<Button>("startButton", 2, "img_StartButton", 1, 3);
  auto continueButton = UI<Button>("continueButton", 2, "img_ContButton", 1, 3);
  auto settingButton = UI<Button>("settingButton", 2, "img_SettButton", 1, 3);

#if ECLIPSEA_DEBUG_WORLD
  {
    auto worldbutton = UI<Button>("worldbutton", 2, "img_createworld", 1, 3);
    worldbutton->Configure()
        .Anchor(AnchorPoint::TopLeft)
        .Parent(nullptr)
        .Posite(0.72f, 0.58f)
        .Scale(0.156f, 0.0416f * widthheight)
        .Sequence(true);

    worldbutton->setOnClick([this]() {
      auto worlde = std::make_unique<WorldEditorStage>();
      sController->changeStage(std::move(worlde));
    });

    Elements->PushElement(std::move(worldbutton));
  }
#endif

  // 配置位置与缩放
  startButton->Configure()
      .Anchor(AnchorPoint::TopLeft)
      .Parent(nullptr)
      .Posite(0.755f, 0.68f)
      .Scale(0.156f, 0.0416f * widthheight)
      .Sequence(true);

  continueButton->Configure()
      .Anchor(AnchorPoint::TopLeft)
      .Parent(nullptr)
      .Posite(0.79f, 0.78f)
      .Scale(0.156f, 0.0416f * widthheight)
      .Sequence(true);

  settingButton->Configure()
      .Anchor(AnchorPoint::TopLeft)
      .Parent(nullptr)
      .Posite(0.825f, 0.88f)
      .Scale(0.156f, 0.0416f * widthheight)
      .Sequence(true);

  // 回调
  startButton->setOnClick([this]() {
    auto gameplay = std::make_unique<GameplayStage>();
    sController->changeStage(std::move(gameplay));
  });

  continueButton->setOnClick([this]() {
    auto story = std::make_unique<ContinueStage>();
    sController->changeStage(std::move(story));
  });

  settingButton->setOnClick([this]() {
    LOG("Setting button clicked");
    auto settings = std::make_unique<SettingsStage>();
    sController->changeStage(std::move(settings));
  });

  // 推入元素管理器
  Elements->PushElement(std::move(startButton));
  Elements->PushElement(std::move(continueButton));
  Elements->PushElement(std::move(settingButton));
}

void MainStage::onExit() {
  // 停止动画、音效
}

void MainStage::onUpdate() {
  Elements->onUpdate(timer->getTotalTime());
  if (phase == MainStagePhase::Idle) {
    auto connector = Elements->find("connector");
    if (connector && connector->isAnimeFinished()) {
      Eclipsea::AudioManager::getInstance().changeBGM("music_lostworld");
      Eclipsea::AudioManager::getInstance().playBGM();
      Elements->removeElement("connector");
    }
  }
}

void MainStage::onRender() { Elements->onRender(); }

bool MainStage::parseEvents(Event *event) {
  Elements->parseEvents(event, timer->getTotalTime());
  return true;
}
