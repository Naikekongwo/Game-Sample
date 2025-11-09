#pragma once

// Union.hpp
// 项目实际的头库

#include "Animation/MyAnimationPipeline.hpp"

#include "Gameplay/Items/ItemConsumable.hpp"
#include "Gameplay/ItemContainer/Backpack.hpp"

#include "Stage/PreloadStage.hpp"
#include "Stage/MainStage.hpp"
#include "Stage/GameplayStage.hpp"

// 模板
template <typename T>
std::unique_ptr<T> UI(std::string id, short layer, short texID, short frameX, short frameY);

#include "template/UIFactory.inl"
