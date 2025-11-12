#ifndef _PURIFIER_STAGE_H_
#define _PURIFIER_STAGE_H_


// Purifier Stage
// 净水器的UI界面

#include "OpenCore/Stage/OverlayStage.hpp"

class PurifierStage : public OverlayStage
{
    public:

    PurifierStage(Timer* timer, StageController* sController);

    bool handlEvents(SDL_Event* event) override;

    void onUpdate() override;

    void onRender() override;

    protected:

    bool constructStage();

    private:
};


#endif //_PURIFIER_STAGE_H_