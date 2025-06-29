#ifndef _OPENCORE_H_
#define _OPENCORE_H_

// openCore.h 引擎核心

#include "graphics/graphics.h"
#include "Timer.h"

#include "../stage/stage.h"
#include "../stage/game_preload.h"

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