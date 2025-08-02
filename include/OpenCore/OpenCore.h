#ifndef _OPENCORE_H_
#define _OPENCORE_H_

// OpenCore.h
// 引擎的核心


#include "Timer.h"
#include "Macros.h"

#include "ResourceManager.h"
#include "SoundEffectManager.h"
#include "Graphics/GraphicsManager.h"
#include "Animation/AnimationManager.h"
#include "Graphics/SpriteManager.h"
#include "Stage/StageController.h"

#include <memory>

class OpenEngine
{
    public:
    bool Run();
    bool Initialize();
    bool MainLoop();
    bool CleanUp();

    private:
    std::unique_ptr<GraphicsInstance> gfxInstance;
    std::unique_ptr<SoundEffectManager> sfxManager;
    std::unique_ptr<StageController> sController;
    std::unique_ptr<Timer> timer;

    ResourceManager* resManager; // 这个不用智能指针
};

#endif //_OPENCORE_H_