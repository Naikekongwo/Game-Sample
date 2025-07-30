#ifndef _OPENCORE_H_
#define _OPENCORE_H_

// OpenCore.h
// 引擎的核心

#include "GfxCore.h"
#include "../Stage/StageController.h"
#include "ResourceManager.h"
#include "Timer.h"

#include <memory>

class OpenEngine
{
    public:
    bool Run();
    bool Initialize();
    bool MainLoop();

    private:
    std::unique_ptr<GFXinstance> gfxInstance;
    std::unique_ptr<SoundEffectManager> sfxManager;
    std::unique_ptr<StageController> sController;
    std::unique_ptr<Timer> timer;

    ResourceManager* resManager; // 这个不用智能指针
};

#endif //_OPENCORE_H_