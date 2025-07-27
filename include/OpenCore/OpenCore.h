#ifndef _OPENCORE_H_
#define _OPENCORE_H_

// OpenCore.h
// 引擎的核心

#include "GfxCore.h"

class OpenEngine
{
    public:
    bool Run();
    bool MainLoop();
    void CleanUp();

    private:
    GFXinstance* gfxInstance;
};

#endif //_OPENCORE_H_