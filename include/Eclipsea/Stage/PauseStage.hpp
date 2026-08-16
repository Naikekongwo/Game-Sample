#pragma once

// PauseStage.hpp
// 暂停页面

#include "World/Stage/OverlayStage.hpp"
#include <memory>

using std::unique_ptr;

class StageManager;

class PauseStage : public OverlayStage
{
  public:
    PauseStage();

    /**
     * @brief OpenCore处理事件的方法
     */
    bool parseEvents(Event *event) override;

    void onEnter() override;
    void initializeComponents() override;
    void onUpdate() override;
    void onRender() override;
    void onExit() override;
    void onDestroy() override;

  private:
};
