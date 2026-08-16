#pragma once

// SettingsStage.hpp
// 设置页面

#include "World/Stage/TopStage.hpp"

enum class SettingsPhase
{
    Entering,
    Ready,
    Exiting
};

class SettingsStage : public TopStage
{
  public:
    SettingsStage();

    void onEnter() override;
    void onUpdate() override;
    bool parseEvents(Event *event) override;
    void initializeComponents() override;
    void onExit() override;
    void onDestroy() override;
    void onRender() override;

  private:
    SettingsPhase phase = SettingsPhase::Entering;
};
