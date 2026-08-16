#pragma once

// ContinueStage.hpp
// 继续页面

#include "World/Stage/TopStage.hpp"
#include <memory>

enum class ContinuePhase
{
    Entering,
    Ready,
    Exiting
};

class ContinueStage : public TopStage
{
  public:
    ContinueStage();

    void onEnter() override;
    void onUpdate() override;
    bool parseEvents(Event *event) override;

    void initializeComponents() override;
    void onExit() override;
    void onDestroy() override;
    void onRender() override;

  private:
    ContinuePhase phase = ContinuePhase::Entering;
};
