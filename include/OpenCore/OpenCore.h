#ifndef _OPENCORE_H_
#define _OPENCORE_H_

// OpenCore.h
// 引擎的核心

#include "GfxCore.h"
#include "../Stage/StageController.h"
#include "ResourceManager.h"
#include "Timer.h"

class OpenEngine
{
    public:
    bool Run();
    bool MainLoop();
    void CleanUp();

    private:
    GFXinstance* gfxInstance;
    ResourceManager* resManager;
    SoundEffectManager* sfxManager;
    StageController* sController;
    Timer* timer;
};

#endif //_OPENCORE_H_