/**
 * @file PauseStage.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2026-05-16
 *
 * @copyright Copyright (c) 2026
 *
 */

#pragma once
#include "OpenCore/World/Stage/OverlayStage.hpp"
#include <memory>

using std::unique_ptr;

class StageManager;

class PauseStage : public OverlayStage
{
  public:
    PauseStage(Timer *timer, StageManager *sController);

    bool handlEvents(SDL_Event *event) override;

    /**
     * @brief OpenCore处理事件的方法
     *
     * @param event
     * @return true
     * @return false
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