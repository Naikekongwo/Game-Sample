#pragma once

// Eclipsea.hpp
// 项目实际的头库

#define music_oceanwaves 1001

#define background_ocean 2001
#define preload_title 2002
#define icon_opencore 2003
#define icon_studio 2004
#define img_connector 2005
#define background_main 2006
#define background_gameplay 2007
#define img_StartButton 2008
#define img_ContButton 2010
#define img_SettButton 2011
#define background_purifier 2009
#define main_title 2012
#define water_drops 2013
#define water_waves 2014
#define img_BackButton 2015

#define font_normal 9001

#include "Animation/MyAnimationPipeline.hpp"

#include "Gameplay/Items/ItemConsumable.hpp"
#include "Gameplay/ItemContainer/Backpack.hpp"


#include "UI/MultiImageBoard.hpp"

#include "Stage/PurifierStage.hpp"
#include "Stage/PreloadStage.hpp"
#include "Stage/MainStage.hpp"
#include "Stage/GameplayStage.hpp"

// 模板
template <typename T>
std::unique_ptr<T> UI(const std::string& id, uint8_t layer, short texID, short frameX, short frameY);

#include "template/UIFactory.inl"


std::unique_ptr<Texture> MakeTexture(uint8_t xCount, uint8_t yCount, short texId);

#include "template/TextureFactory.inl"