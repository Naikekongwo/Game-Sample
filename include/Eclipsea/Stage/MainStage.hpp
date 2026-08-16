#pragma once

// MainStage.hpp
// 定义主页面类的头文件

#include "World/Stage/StageManager.hpp"

enum class MainStagePhase
{
    Entering,
    Idle,
    Exposure,
    Exiting
};

class StageManager;
class ElementManager;

class MainStage : public OverlayStage
{
  public:
    // 构造函数（timer / sController 由 StageManager 自动注入）
    MainStage();

    // 完成继承
    bool parseEvents(Event *event) override;

    void initializeComponents() override;
    void onEnter() override;
    void onExit() override;

    void onUpdate() override;
    void onRender() override;

  private:
    MainStagePhase phase = MainStagePhase::Entering;

    // 工具函数
    void setupBackground();
    void setupTitle();
    void setupButtons();
};
