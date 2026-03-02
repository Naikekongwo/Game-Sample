#ifndef _PURIFIER_STAGE_H_
#define _PURIFIER_STAGE_H_

// Purifier Stage
// 净水器的UI界面

#include "OpenCore/World/Stage/OverlayStage.hpp"

enum class PurifierPhase
{
    Normal,
    Exiting,
    Finished
};

class PurifierStage : public OverlayStage
{
  public:
    PurifierStage(Timer *timer, StageManager *sController);

    bool handlEvents(SDL_Event *event) override;
    void onUpdate() override;
    void onRender() override;

    void onDestroy() override;

    void onEnter() override;
    void onExit() override;

  protected:
    bool buildStage();

  private:
    PurifierPhase phase = PurifierPhase::Normal;
};

#endif //_PURIFIER_STAGE_H_