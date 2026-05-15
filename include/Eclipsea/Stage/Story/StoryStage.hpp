#pragma once

#include "OpenCore/World/Stage/OverlayStage.hpp"

class TypeWriter;

enum class StoryStatus
{
    Loading,
    Ready,
    Intro,
    Launching
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
    short stageIndex = 0;

    StoryStatus sStatus = StoryStatus::Loading;

    void func_intro();

    // Intro 子阶段处理函数
    void handleIntroUpdate();
    void handleIntroScrollText(TypeWriter *typeWriter, const std::string &text);
    void handleIntroCenterText(TypeWriter *typeWriter);
    void handleIntroVisualScene(TypeWriter *typeWriter);
    void handleIntroFadeAudio(TypeWriter *typeWriter);
    void handleIntroLaunch();
};