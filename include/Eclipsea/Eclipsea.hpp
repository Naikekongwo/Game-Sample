#pragma once

// Eclipsea.hpp
// 项目实际的头库

#include "Animation/MyAnimationPipeline.hpp"

#include "Gameplay/Items/ItemConsumable.hpp"
#include "Gameplay/ItemContainer/Backpack.hpp"


#include "UI/MultiImageBoad.hpp"

#include "Stage/PreloadStage.hpp"
#include "Stage/MainStage.hpp"
#include "Stage/GameplayStage.hpp"

// 模板
template <typename T>
std::unique_ptr<T> UI(const std::string& id, uint8_t, short texID, short frameX, short frameY);

#include "template/UIFactory.inl"


std::unique_ptr<Texture> MakeTexture(uint8_t xCount, uint8_t yCount, short texId);

#include "template/TextureFactory.inl"