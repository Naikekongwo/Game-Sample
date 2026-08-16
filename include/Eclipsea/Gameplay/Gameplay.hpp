#pragma once

// Gameplay.hpp
// 项目级玩法命名空间
//
// 新版 OpenCore 已移除旧的 Gameplay 命名空间（EntityReg / ItemMgr）。
// 本项目在项目层重新提供，并保留别名 Gameplay 以兼容旧玩法代码。

#include "Eclipsea/Gameplay/Entity/EntityRegister.hpp"
#include "Eclipsea/Gameplay/Backpack/ItemManager.hpp"

namespace EclipseaGameplay
{
inline EntityRegister &EntityReg = EntityRegister::getInstance();
inline ItemManager    &ItemMgr   = ItemManager::getInstance();
} // namespace EclipseaGameplay

// 兼容旧代码的命名空间别名
namespace Gameplay = EclipseaGameplay;

// 原引擎 OpenEngine::getServerWorldController() 的等价物
// （新引擎已移除世界控制器，这里以项目级单例提供）
#include "Eclipsea/Gameplay/WorldController/WorldController.hpp"
inline WorldController &getServerWorldController()
{
    return WorldController::getInstance();
}
