#ifndef _OPENCORE_H_
#define _OPENCORE_H_

// OpenCore.hpp
// 引擎的核心

// 对于调试模式的宏定义
#define DEBUG_OFF 0
#define DEBUG_WATERWAVE 1
#define DEBUG_MAP 1213
#define DEBUG_MAIN 520

#define DEBUG_MODE DEBUG_MAIN

#include "Macros.hpp"
#include "Timer.hpp"

#include "Graphics/Manager/GraphicsManager.hpp"
#include "OpenCore/Concepts/OpenCore_Vec3.hpp"
#include "OpenCore/Concepts/OpenCore_Wave.hpp"
#include "ResourceManager.hpp"
#include "SoundEffectManager.hpp"

#include "Gameplay/Manager/EntityManager.hpp"
#include "Map/Manager/MapManager.hpp"

// 内核所对应的在整个游戏生命周期中只可能出现一次的管理器（即单例）的命名空间
namespace OpenCoreManagers
{
inline ResourceManager &ResManager = ResourceManager::getInstance();
inline GraphicsManager &GFXManager = GraphicsManager::getInstance();
inline SoundEffectManager &SFXManager = SoundEffectManager::getInstance();
} // namespace OpenCoreManagers

// 游戏对应的模块单例
namespace Gameplay
{
inline MapManager &MapMgr = MapManager::getInstance();
inline PlayerEntity &Player = PlayerEntity::getInstance();
} // namespace Gameplay

#include "Animation/AnimationPipeline.hpp"
#include "Animation/Manager/AnimationManager.hpp"
#include "Graphics/Configurator/DrawableConfigurator.hpp"
#include "Graphics/Manager/ElementManager.hpp"
#include "Graphics/Manager/ScaleManager.hpp"
#include "Stage/StageManager.hpp"

#include <memory>

class OpenEngine final
{
  public:
    // 单例的获取函数
    static OpenEngine &getInstance();

    bool Run();
    bool Initialize();
    bool MainLoop();
    bool CleanUp();

  private:
    std::unique_ptr<StageManager> sController;
    std::unique_ptr<Timer> timer;
};

#endif //_OPENCORE_H_