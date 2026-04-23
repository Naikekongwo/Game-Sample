// MainStage.hpp
// 定义主页面类的头文件
#ifndef _MAINSTAGE_H_
#define _MAINSTAGE_H_

#include "OpenCore/World/Stage/StageManager.hpp"

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
    // 构造函数
    MainStage(Timer *timer, StageManager *sController);

    // 完成继承
    bool handlEvents(SDL_Event *event) override;

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

#endif //_MAINSTAGE_H_