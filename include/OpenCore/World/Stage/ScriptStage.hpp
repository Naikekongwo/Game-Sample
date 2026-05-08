#pragma once

#include "OverlayStage.hpp"
#include "Stage.hpp"
#include <memory>

class ScriptStage : public OverlayStage
{
    ScriptStage(Timer *timer, StageManager *sController);
    bool handlEvents(SDL_Event *event) override;
    bool parseEvents(Event *event) override;

    void onEnter() override;
    void initializeComponents() override;

    void onUpdate() override;

    void onRender() override;

    void onExit() override;

    void onDestroy() override;
};