#pragma once

#include "World/Stage/StageManager.hpp"

// WorldEditorStage.hpp
// 世界编辑器场景的头

// 前向声明
class StageManager;
class ElementManager;

class WorldEditorStage : public BaseStage
{
  public:
    // 构造函数（timer / sController 由 StageManager 自动注入）
    WorldEditorStage();

    // 完成继承
    bool parseEvents(Event *event) override;

    void initializeComponents() override;
    void onEnter() override;

    void onUpdate() override;

    void onRender() override;

    // 准备函数
    void Init();

  private:
};
