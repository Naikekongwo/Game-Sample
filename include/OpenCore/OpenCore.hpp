#ifndef _OPENCORE_H_
#define _OPENCORE_H_

// OpenCore.hpp
// 引擎的核心

// 对于调试模式的宏定义
#define DEBUG_OFF 0
#define DEBUG_WATERWAVE 1
#define DEBUG_MAP 1213

#define DEBUG_MODE DEBUG_OFF


#include "Timer.hpp"
#include "Macros.hpp"

#include "OpenCore/Concepts/Vec3.hpp"
#include "OpenCore/Concepts/WaterSimpler.hpp"

#include "ResourceManager.hpp"
#include "SoundEffectManager.hpp"
#include "Graphics/Manager/GraphicsManager.hpp"

#include "Gameplay/Manager/ItemManager.hpp"
#include "OpenCore/Map/Manager/MapManager.h"

// 内核所对应的在整个游戏生命周期中只可能出现一次的管理器（即单例）的命名空间
namespace OpenCoreManagers
{
    inline ResourceManager& ResManager = ResourceManager::getInstance();
    inline GraphicsManager& GFXManager = GraphicsManager::getInstance();
    inline SoundEffectManager& SFXManager = SoundEffectManager::getInstance();
}

// 游戏对应的模块单例
namespace Gameplay
{
    inline ItemManager& ItemMgr = ItemManager::getInstance();
    inline MapManager& MapMgr = MapManager::getInstance();
}

#include "Graphics/Manager/ScaleManager.hpp"
#include "Animation/AnimationPipeline.hpp"
#include "Animation/Manager/AnimationManager.hpp"
#include "Graphics/Configurator/DrawableConfigurator.hpp"
#include "Graphics/Manager/ElementManager.hpp"
#include "Stage/StageController.hpp"

#include <memory>


class OpenEngine final
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
};

#endif //_OPENCORE_H_