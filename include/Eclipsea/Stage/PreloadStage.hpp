#pragma once

// PreloadStage.hpp
// 预加载页面（基于新 OpenCore，使用 SequentialPipeline 顺序任务）

#include "World/Stage/StageManager.hpp"

class StageManager;
class ElementManager;

class PreloadStage : public OverlayStage
{
  public:
    PreloadStage();

    // 生命周期
    void onEnter() override;
    void onExit() override;
    void onDestroy() override;

    // 基类接口
    bool parseEvents(Event *event) override;
    void initializeComponents() override;
    void onUpdate() override;
    void onRender() override;

  protected:
    void buildLoadingUI();
    void buildTitle();
};
