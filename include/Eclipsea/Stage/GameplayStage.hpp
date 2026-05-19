#pragma once

#include "OpenCore/World/Stage/StageManager.hpp"

// GameplayStage.hpp
// 游戏场景的头

// 前向声明
class StageManager;
class ElementManager;

class GameplayStage : public BaseStage
{
  public:
    // 构造函数
    GameplayStage(Timer *timer, StageManager *sController);

    // 完成继承
    bool handlEvents(SDL_Event *event) override;

    bool parseEvents(Event *event) override;

    void initializeComponents() override;

    void onUpdate() override;

    void onEnter() override { initializeComponents(); }
    void onRender() override;

    // 准备函数
    void Init();

  private:
};
