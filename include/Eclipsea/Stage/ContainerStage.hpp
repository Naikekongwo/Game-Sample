/**
 * @file ContainerStage.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2026-05-15
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "OpenCore/World/Stage/OverlayStage.hpp"

enum class ContainerPhase
{
    Normal,
    Exiting,
    Finished
};

class ContainerStage : public OverlayStage
{
  public:
    ContainerStage(Timer *timer, StageManager *sController,
                   shared_ptr<Backpack> backpack);

    bool handlEvents(SDL_Event *event) override;
    bool parseEvents(Event *event) override;

    void initializeComponents() override;

    void onUpdate() override;
    void onRender() override;

    void onDestroy() override;
    void onEnter() override;
    void onExit() override;

  private:
    ContainerPhase phase = ContainerPhase::Normal;

    weak_ptr<Backpack> m_backpack;
};
