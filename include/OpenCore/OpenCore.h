#ifndef _OPENCORE_H_
#define _OPENCORE_H_

// OpenCore.h
// 引擎的核心


#include "Timer.h"
#include "Macros.h"

#include "ResourceManager.h"
#include "SoundEffectManager.h"
#include "Graphics/GraphicsManager.h"
#include "Graphics/ScaleManager.h"
#include "Animation/AnimationManager.h"
#include "Graphics/ElementManager.h"
#include "Stage/StageController.h"

#include <memory>

namespace OpenCoreManagers
{
    inline ResourceManager& ResManager = ResourceManager::getInstance();
    inline GraphicsManager& GFXManager = GraphicsManager::getInstance();
    inline SoundEffectManager& SFXManager = SoundEffectManager::getInstance();
}

class OpenEngine
{
    public:
    // 单例的获取函数
    static OpenEngine& getInstance();

    bool Run();
    bool Initialize();
    bool MainLoop();
    bool CleanUp();

    private:
    std::unique_ptr<StageController> sController;
    std::unique_ptr<Timer> timer;

    // 所有使用单例的类不使用智能指针
    // SoundEffectManager* sfxManager;
    // ResourceManager* resManager;
    // GraphicsManager* gfxInstance;

    // 所有裸指针都已经废除，既然是单例那么就直接调用
};

#endif //_OPENCORE_H_