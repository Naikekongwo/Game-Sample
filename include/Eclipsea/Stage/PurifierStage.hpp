#ifndef _PURIFIER_STAGE_H_
#define _PURIFIER_STAGE_H_


// Purifier Stage
// 净水器的UI界面

#include "OpenCore/Stage/StageController.hpp"

class PurifierStage : public OverlayStage
{
    public:

    PurifierStage(Timer* timer, StageController* sController);

    bool handlEvents(SDL_Event* event) override;

    void onUpdate() override;

    void onRender() override;

    private:
};


#endif //_PURIFIER_STAGE_H_