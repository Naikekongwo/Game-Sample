#pragma once

#ifndef _PURIFIER_STAGE_H_
#define _PURIFIER_STAGE_H_

// PurifierStage.hpp
// 净水器的UI界面

#include "World/Stage/OverlayStage.hpp"

#include <memory>

class ItemSprite;

enum class PurifierPhase
{
    Normal,
    Selecting,
    Exiting,
    Finished
};

class PurifierStage : public OverlayStage
{
  public:
    PurifierStage();

    bool parseEvents(Event *event) override;
    void initializeComponents() override;
    void onUpdate() override;
    void onRender() override;

    void onDestroy() override;

    void onEnter() override;
    void onExit() override;

  protected:
    bool buildStage();

  private:
    PurifierPhase phase = PurifierPhase::Normal;

    unique_ptr<ItemSprite> ItemPickedUp;
};

#endif //_PURIFIER_STAGE_H_
