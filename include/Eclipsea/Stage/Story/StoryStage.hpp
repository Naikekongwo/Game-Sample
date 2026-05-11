#pragma once

#include "OpenCore/World/Stage/OverlayStage.hpp"

enum class StoryStatus
{
    Loading,
    Ready,
    Intro
};

class StoryStage : public OverlayStage
{
  public:
    // 构造函数
    StoryStage(Timer *timer, StageManager *sController);

    void onEnter() override;

    // 完成继承
    bool handlEvents(SDL_Event *event) override;

    bool parseEvents(Event *event) override;

    void initializeComponents() override;

    void onUpdate() override;

    void onRender() override;

  private:
    StoryStatus sStatus = StoryStatus::Loading;

    void func_intro();
};