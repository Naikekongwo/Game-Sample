#ifndef _OPENCORE_H_
#define _OPENCORE_H_

// OpenCore.hpp
// 引擎的核心

// 对于调试模式的宏定义

enum DebugFlags
{
    DEBUG_OFF = 0,
    DEBUG_WATERWAVE = 1 << 0,  // 0x01
    DEBUG_MAP = 1 << 1,        // 0x02
    DEBUG_MAIN = 1 << 2,       // 0x04
    DEBUG_PASS_VIDEO = 1 << 3, // 0x08
    DEBUG_COPYRIGHT = 1 << 4
};

constexpr int DEBUG_MODE = DEBUG_COPYRIGHT | DEBUG_MAIN;

#include "Macros.hpp"
#include "Timer.hpp"

#include "Graphics/Manager/GraphicsManager.hpp"
#include "OpenCore/Concepts/OpenCore_Spiral.hpp"
#include "OpenCore/Concepts/OpenCore_Vec3.hpp"
#include "OpenCore/Concepts/OpenCore_Wave.hpp"
#include "ResourceManager.hpp"
#include "SettingsManager.hpp"
#include "SoundEffectManager.hpp"

#include "Map/Manager/MapManager.hpp"

// 内核所对应的在整个游戏生命周期中只可能出现一次的管理器（即单例）的命名空间
namespace OpenCoreManagers
{
inline ResourceManager &ResManager = ResourceManager::getInstance();
inline GraphicsManager &GFXManager = GraphicsManager::getInstance();
inline SoundEffectManager &SFXManager = SoundEffectManager::getInstance();
inline SettingsManager &SetManager = SettingsManager::getInstance();
} // namespace OpenCoreManagers

// 游戏对应的模块单例
namespace Gameplay
{
inline MapManager &MapMgr = MapManager::getInstance();
} // namespace Gameplay

#include "Animation/AnimationPipeline.hpp"
#include "Animation/Manager/AnimationManager.hpp"
#include "Graphics/Configurator/DrawableConfigurator.hpp"
#include "Graphics/Manager/ElementManager.hpp"
#include "Stage/StageManager.hpp"

#include <memory>

using std::make_shared;
using std::shared_ptr;
using std::string;

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
    unique_ptr<StageManager> sController;
    unique_ptr<Timer> timer;
};

#endif //_OPENCORE_H_