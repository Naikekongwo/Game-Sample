#pragma once

// SettingsStage.hpp
// 设置页面

#include "OpenCore/World/Stage/TopStage.hpp"

enum class SettingsPhase
{
    Entering,
    Ready,
    Exiting
};

class SettingsStage : public TopStage
{
  public:
    SettingsStage(Timer *timer, StageManager *sController);

    void onEnter() override;
    void onUpdate() override;
    bool handlEvents(SDL_Event *event) override;
    void onExit() override;
    void onDestroy() override;
    void onRender() override;

  private:
    SettingsPhase phase = SettingsPhase::Entering;
};