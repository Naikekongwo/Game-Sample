#pragma once

// Eclipsea.hpp
// 项目实际的头库（基于新 OpenCore 26.1）

#define music_oceanwaves 1001
#define se_waterdrops 1002
#define music_lostworld 1003
#define music_someoneinthedark 1004

#define preload_title 2002
#define icon_opencore 2003
#define icon_studio 2004
#define img_connector 2005
#define background_main 2006
#define desset_top 2020
#define base_sky 2021
#define background_gameplay 2007
#define img_StartButton 2008
#define img_ContButton 2010
#define img_SettButton 2011
#define background_purifier 2009
#define main_title 2012
#define water_drops 2013
#define water_waves 2014
#define img_BackButton 2015
#define wasteland 2016
#define oriwater 2017
#define copyright 2018
#define item_purifier 2019
#define cities_top 2022
#define button_border 2023
#define stone_background 2024
#define img_createworld 2025
#define img_scrollbutton 2026
#define img_itemcontain 2027
#define img_settingstitle 2028
#define img_musicvolume 2029
#define img_sfxvolume 2030
#define pad_copyright 2031
#define img_renderd 2032
#define img_multi 2033
#define img_checkbox 2034
#define terrain_major 2035
#define player_texture 2036
#define stage_continue_title 2037
#define items_sets00 2038
#define chao_texture 2039
#define ui_img_healthbar 2040
#define effects_water_bubbles 2042
#define ui_bg_purifier 2043
#define terrain_terrain 2044
#define entity_shadow 2045
#define background_typewriter 2046
#define img_insidehouse 2047
#define btn_backmain 2048
#define btn_pause_settings 2049
#define effects_explosion 2050
#define effects_flames 2051
#define symbols 2052
#define story_plane 3001
#define story_ruinedcity 3002
#define story_starsky 3003

#define font_normal 9001
#define font_ch 9002

// 调试开关：世界编辑器按钮（原 DEBUG_MAP 模式）
#ifndef ECLIPSEA_DEBUG_WORLD
#define ECLIPSEA_DEBUG_WORLD 0
#endif

#include "OpenCore.hpp"

#include "Stage/ContainerStage.hpp"
#include "Stage/DualGameplayStage.hpp"
#include "Stage/GameplayStage.hpp"
#include "Stage/MainStage.hpp"
#include "Stage/PauseStage.hpp"
#include "Stage/PreloadStage.hpp"
#include "Stage/PurifierStage.hpp"
#include "Stage/WorldEditorStage.hpp"

#include "Stage/Story/StoryStage.hpp"

#include "Stage/MainStage/ContinueStage.hpp"
#include "Stage/MainStage/SettingsStage.hpp"

class EclipseaApp
{
  public:
    bool StartUp();
    bool CleanUp();

  private:
};
