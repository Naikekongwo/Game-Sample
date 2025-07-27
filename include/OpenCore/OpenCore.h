#ifndef _OPENCORE_H_
#define _OPENCORE_H_

// OpenCore.h
// 引擎的核心

#include "GfxCore.h"
#include "Timer.h"

class OpenEngine
{
    public:
    bool Run();
    bool MainLoop();
    void CleanUp();

    private:
    Timer* timer;
    GFXinstance* gfxInstance;
};

#endif //_OPENCORE_H_