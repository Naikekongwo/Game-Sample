#ifndef _OPENCORE_H_
#define _OPENCORE_H_

#include "graphics.h"
#include "stage/stage_controller.h"
#include "Timer.h"

class openCore
{
    public:
    bool Start();
    bool MainLoop();
    void CleanUp();

    private:
    graphics* graphic;
};


#endif //_OPENCORE_H_