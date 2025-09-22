#ifndef _OPENCORE_H_
#define _OPENCORE_H_

// OpenCore.hpp
// 引擎的核心

// 对于调试模式的宏定义
#define DEBUG_OFF 0
#define DEBUG_MAP 1213

#define DEBUG_MODE DEBUG_MAP


#include "Timer.hpp"
#include "Macros.hpp"

#include "ResourceManager.hpp"
#include "SoundEffectManager.hpp"
#include "Graphics/GraphicsManager.hpp"
#include "Graphics/ScaleManager.hpp"
#include "Animation/AnimationPipeline.hpp"
#include "Animation/AnimationManager.hpp"
#include "Graphics/DrawableConfigurator.hpp"
#include "Graphics/ElementManager.hpp"
#include "Stage/StageController.hpp"

#include <memory>

// 内核所对应的在整个游戏生命周期中只可能出现一次的管理器（即单例）的命名空间
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