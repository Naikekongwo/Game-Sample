#pragma once

// SettingsStage.hpp
// 设置页面

#include "OpenCore/World/Stage/TopStage.hpp"

enum class ContinuePhase
{
    Entering,
    Ready,
    Exiting
};

class ContinueStage : public TopStage
{
  public:
    ContinueStage(Timer *timer, StageManager *sController);

    void onEnter() override;
    void onUpdate() override;
    bool handlEvents(SDL_Event *event) override;
    void onExit() override;
    void onDestroy() override;
    void onRender() override;

  private:
    ContinuePhase phase = ContinuePhase::Entering;
};