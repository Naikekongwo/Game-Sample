#pragma once

// DualGameplayStage.hpp
// 双人游戏页面

#include "Core/Event/EventManager.hpp"
#include "World/Stage/StageManager.hpp"

class DualGameplayStage : public BaseStage
{
  public:
    DualGameplayStage();

    /**
     * @brief 事件处理（OpenCore）
     */
    bool parseEvents(Event *event) override;

    void initializeComponents() override;
    void onEnter() override;
    void onUpdate() override;
    void onRender() override;

    // 准备函数
    void Init();

  private:
};
