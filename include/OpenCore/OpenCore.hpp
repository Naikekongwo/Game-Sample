#ifndef _OPENCORE_H_
#define _OPENCORE_H_

// OpenCore.hpp
// 引擎的核心

// 对于调试模式的宏定义

#include "Runtime/Gameplay/Entity/EntityRegister.hpp"
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

#include "OpenCore/Core/Macros.hpp"
#include "OpenCore/Core/Timer.hpp"

#include "OpenCore/Asset/ResourceManager.hpp"
#include "OpenCore/Core/GameInfo.hpp"
#include "OpenCore/Core/Math/OpenCore_Spiral.hpp"
#include "OpenCore/Core/Math/OpenCore_Vec3.hpp"
#include "OpenCore/Core/Math/OpenCore_Wave.hpp"
#include "OpenCore/Runtime/Audio/SoundEffectManager.hpp"
#include "OpenCore/Runtime/Config/SettingsManager.hpp"
#include "OpenCore/Runtime/Gameplay/Entity/EntityRegister.hpp"
#include "OpenCore/Runtime/Gameplay/WorldController/IWorldController.hpp"
#include "OpenCore/Runtime/Graphics/Manager/GraphicsManager.hpp"

#include "OpenCore/Core/Thread/ThreadManager.hpp"
#include "OpenCore/World/Map/Manager/MapManager.hpp"

// 内核所对应的在整个游戏生命周期中只可能出现一次的管理器（即单例）的命名空间

namespace OpenCoreManagers
{
inline ThreadManager &ThrManager = ThreadManager::getInstance();
inline ResourceManager &ResManager = ResourceManager::getInstance();
inline GraphicsManager &GFXManager = GraphicsManager::getInstance();
inline SoundEffectManager &SFXManager = SoundEffectManager::getInstance();
inline SettingsManager &SetManager = SettingsManager::getInstance();
} // namespace OpenCoreManagers

// 游戏对应的模块单例

namespace Gameplay
{
inline EntityRegister &EntityReg = EntityRegister::getInstance();
}

#include "OpenCore/Runtime/Animation/AnimationPipeline.hpp"
#include "OpenCore/Runtime/Animation/Manager/AnimationManager.hpp"
#include "OpenCore/Runtime/Graphics/Configurator/DrawableConfigurator.hpp"
#include "OpenCore/Runtime/Graphics/Manager/ElementManager.hpp"
#include "OpenCore/World/Stage/StageManager.hpp"

// 模板
template <typename T>
unique_ptr<T> UI(const std::string &id, uint8_t layer, short texID,
                 short frameX, short frameY);

#include "OpenCore/Runtime/Graphics/Factory/UIFactory.inl"

unique_ptr<Texture> MakeTexture(uint8_t xCount, uint8_t yCount, short texId);

#include "OpenCore/Runtime/Graphics/Factory/TextureFactory.inl"

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

    // 注册游戏类
    bool GameRegistry(unique_ptr<GameInfo> gameInfo);

    // 工具函数
    Timer *getTimer() const noexcept { return timer.get(); }
    StageManager *getStageController() const noexcept
    {
        return sController.get();
    }

    GameInfo *getGameInfo() { return gameInfo.get(); }
    EntityRegister &getEntityRegister() { return Gameplay::EntityReg; }

  private:
    unique_ptr<GameInfo> gameInfo = std::make_unique<GameInfo>();
    unique_ptr<StageManager> sController;
    unique_ptr<Timer> timer;
};

#endif //_OPENCORE_H_