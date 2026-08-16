#pragma once

// ContainerStage.hpp
// 容器（背包）界面

#include "World/Stage/OverlayStage.hpp"

#include <memory>

using std::shared_ptr;
using std::weak_ptr;

class Backpack;

enum class ContainerPhase
{
    Normal,
    Exiting,
    Finished
};

class ContainerStage : public OverlayStage
{
  public:
    ContainerStage(shared_ptr<Backpack> backpack);

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
