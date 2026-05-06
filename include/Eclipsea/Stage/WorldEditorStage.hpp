#pragma once

#include "OpenCore/World/Stage/StageManager.hpp"

// WorldEditorStage.hpp
// 游戏场景的头

// 前向声明
class StageManager;
class ElementManager;

class WorldEditorStage : public BaseStage
{
  public:
    // 构造函数
    WorldEditorStage(Timer *timer, StageManager *sController);

    // 完成继承
    bool handlEvents(SDL_Event *event) override;

    bool parseEvents(Event *event) override;

    void initializeComponents() override;

    void onUpdate() override;

    void onRender() override;

    // 准备函数
    void Init();

  private:
};
