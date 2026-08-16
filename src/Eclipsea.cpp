#include "Eclipsea/Eclipsea.hpp"
#include "Eclipsea/Core/AudioManager.hpp"
#include "Eclipsea/Core/EclipseaTextures.hpp"
#include "Eclipsea/Gameplay/TextureMeta.hpp"
#include "Eclipsea/Gameplay/Gameplay.hpp"
#include "Core/Info/ResourceInfo.hpp"
#include "Eclipsea/Gameplay/Backpack/ItemManager.hpp"
#include "Eclipsea/Gameplay/Backpack/Iteminfo.hpp"
#include "Eclipsea/Gameplay/Entity/EntityRegister.hpp"
#include <exception>
#include <memory>
#include <stdexcept>
#include <system_error>

// 便捷：同时向 PackageManager 注册纹理，并在数字ID桥中绑定名称
static void RegisterTexture(PackageManager *package, short id,
                            const char *name, const char *path)
{
    package->registerResource(RscTexture, name, path);
    EclipseaTextures::getInstance().bindTexture(id, name);
}

// 便捷：注册字体
static void RegisterFont(PackageManager *package, const char *name,
                         const char *path)
{
    package->registerResource(RscFont, name, path);
}

// 便捷：注册音乐 / 音效
static void RegisterMusic(short id, const char *path)
{
    Eclipsea::AudioManager::getInstance().registerMusic(id, path);
}
static void RegisterSound(short id, const char *path)
{
    Eclipsea::AudioManager::getInstance().registerSound(id, path);
}

bool EclipseaApp::StartUp()
{
    (void)OpenEngine::getInstance();

    OpenEngine &engine = OpenEngine::getInstance();

    // ── 图形信息 ─────────────────────────────────────────────
    GraphicsInfo gInfo;
    gInfo.keepRatio      = true;
    gInfo.nearestScaling = false;
    gInfo.resizable      = false;
    gInfo.targetFrameRate = 144;
    gInfo.resolutionWidth  = 1920;
    gInfo.resolutionHeight = 1080;

    // ── 资源信息 ─────────────────────────────────────────────
    ResourceInfo rInfo;
    rInfo.keepStructureWhenPackaging = true;
    rInfo.packageOnly                = false;
    rInfo.textureResampleFactor      = 1.0f;

    auto eclipseaInfo = std::make_unique<GameInfo>();

    eclipseaInfo->gameName     = "Eclipsea";
    eclipseaInfo->beta         = true;
    eclipseaInfo->version_major = 1;
    eclipseaInfo->version_minor = 0;
    eclipseaInfo->entranceStage = std::make_unique<PreloadStage>();

    eclipseaInfo->_graphicsInfo = gInfo;
    eclipseaInfo->_resourceInfo = rInfo;

    engine.GameRegistry(std::move(eclipseaInfo));

    if (!engine.Initialize())
    {
        LOG("Engine initialization was failed.");
        return false;
    }

    auto Package = engine.getPackageManager();

    // ── 注册纹理 ─────────────────────────────────────────────
    RegisterTexture(Package, preload_title, "preload_title",
                    "assets/ui/Preload_Title.png");
    RegisterTexture(Package, icon_opencore, "icon_opencore",
                    "assets/ui/icon_opencore.png");
    RegisterTexture(Package, icon_studio, "icon_studio",
                    "assets/ui/icon_studio.png");
    RegisterTexture(Package, img_connector, "img_connector",
                    "assets/ui/preload_menu_connector.png");
    RegisterTexture(Package, background_main, "background_main",
                    "assets/backgrounds/main_background.png");
    RegisterTexture(Package, background_gameplay, "background_gameplay",
                    "assets/backgrounds/gameplay_background.png");
    RegisterTexture(Package, img_StartButton, "img_StartButton",
                    "assets/ui/main/Button_Start.png");
    RegisterTexture(Package, background_purifier, "background_purifier",
                    "assets/ui/purifier_background.png");
    RegisterTexture(Package, img_ContButton, "img_ContButton",
                    "assets/ui/main/Button_Continue.png");
    RegisterTexture(Package, img_SettButton, "img_SettButton",
                    "assets/ui/main/Button_Settings.png");
    RegisterTexture(Package, main_title, "main_title",
                    "assets/ui/Title_Main.png");
    RegisterTexture(Package, water_drops, "water_drops",
                    "assets/ui/animation_waterdrops.png");
    RegisterTexture(Package, water_waves, "water_waves",
                    "assets/ui/animation_waterwaves.png");
    RegisterTexture(Package, img_BackButton, "img_BackButton",
                    "assets/ui/settings/Button_Back.png");
    RegisterTexture(Package, copyright, "copyright",
                    "assets/ui/icon_copyright.png");
    RegisterTexture(Package, item_purifier, "item_purifier",
                    "assets/ui/purifier/purifier.png");
    RegisterTexture(Package, desset_top, "desset_top",
                    "assets/backgrounds/dessert_top.png");
    RegisterTexture(Package, base_sky, "base_sky",
                    "assets/backgrounds/base_sky.png");
    RegisterTexture(Package, cities_top, "cities_top",
                    "assets/backgrounds/cities_top.png");
    RegisterTexture(Package, button_border, "button_border",
                    "assets/ui/rect_border.png");
    RegisterTexture(Package, stone_background, "stone_background",
                    "assets/ui/stone_background.png");
    RegisterTexture(Package, img_createworld, "img_createworld",
                    "assets/ui/main/Button_Create.png");
    RegisterTexture(Package, img_scrollbutton, "img_scrollbutton",
                    "assets/ui/scrollbutton.png");
    RegisterTexture(Package, img_itemcontain, "img_itemcontain",
                    "assets/ui/item_container.png");
    RegisterTexture(Package, pad_copyright, "pad_copyright",
                    "assets/ui/pad_copyright.png");
    RegisterTexture(Package, img_checkbox, "img_checkbox",
                    "assets/ui/checkbox_classic.png");
    RegisterTexture(Package, player_texture, "player_texture",
                    "assets/entity/player_texture.png");
    RegisterTexture(Package, items_sets00, "items_sets00",
                    "assets/items/items_sets00.png");
    RegisterTexture(Package, chao_texture, "chao_texture",
                    "assets/entity/chao_texture.png");
    RegisterTexture(Package, ui_img_healthbar, "ui_img_healthbar",
                    "assets/ui/gameplay/healthbar.png");
    RegisterTexture(Package, effects_water_bubbles, "effects_water_bubbles",
                    "assets/effects/waterparticles.png");
    RegisterTexture(Package, ui_bg_purifier, "ui_bg_purifier",
                    "assets/ui/purifier/purifier_background.png");
    RegisterTexture(Package, terrain_terrain, "terrain_terrain",
                    "assets/terrain/terrain.png");
    RegisterTexture(Package, entity_shadow, "entity_shadow",
                    "assets/entity/entity_shadow.png");
    RegisterTexture(Package, background_typewriter, "background_typewriter",
                    "assets/ui/background_typewriter.png");
    RegisterTexture(Package, img_insidehouse, "img_insidehouse",
                    "assets/story/img_insidehouse.png");
    RegisterTexture(Package, btn_backmain, "btn_backmain",
                    "assets/ui/pause/btn_backmain.png");
    RegisterTexture(Package, btn_pause_settings, "btn_pause_settings",
                    "assets/ui/pause/btn_settings.png");
    RegisterTexture(Package, effects_explosion, "effects_explosion",
                    "assets/effects/explosion.png");
    RegisterTexture(Package, effects_flames, "effects_flames",
                    "assets/effects/flames.png");
    RegisterTexture(Package, symbols, "symbols", "assets/ui/symbols.png");
    RegisterTexture(Package, story_plane, "story_plane",
                    "assets/story/img_plane.png");
    RegisterTexture(Package, story_ruinedcity, "story_ruinedcity",
                    "assets/story/img_ruinedcity.png");
    RegisterTexture(Package, story_starsky, "story_starsky",
                    "assets/story/img_starsky.png");

    // ── 注册字体 ─────────────────────────────────────────────
    RegisterFont(Package, "9001", "assets/ui/font/OpenCoreFont.ttf");
    RegisterFont(Package, "9002", "assets/ui/font/OpenCoreCHFont.ttf");
    RegisterFont(Package, "OpenCoreFont", "assets/ui/font/OpenCoreFont.ttf");

    // ── 注册音频（数字ID → 文件路径）──────────────────────────
    RegisterMusic(music_oceanwaves, "assets/audio/bgm_oceanwaves.mp3");
    RegisterSound(se_waterdrops, "assets/audio/fx_waterdrops.mp3");
    RegisterMusic(music_lostworld, "assets/audio/bgm_lostworld.mp3");
    RegisterMusic(music_someoneinthedark,
                  "assets/audio/bgm_someoneinthedark.mp3");

    // ── 初始化音频 ───────────────────────────────────────────
    Eclipsea::AudioManager::getInstance().Init();

#pragma region 注册实体
    auto &entityReg = EntityRegister::getInstance();

    EntityInfo player = EntityInfo::makeCharacter(
        PLAYER_ENTITYTYPE, player_texture, 8, true, 1.0f);
    entityReg.registerEntity(player);

    EntityInfo player1 =
        EntityInfo::makeCharacter(2, chao_texture, 8, true, 1.0f);
    entityReg.registerEntity(player1);

    EntityInfo purifier{100, {{2044, 8, 8}, 0, 1}, {}, 4, 0, 3.0f, 3.0f};
    entityReg.registerEntity(purifier);

    EntityInfo storage{101, {{2044, 8, 8}, 0, 4}, {}, 4, 0, 2.0f, 2.0f};
    entityReg.registerEntity(storage);
#pragma endregion

#pragma region 注册物品相关内容
    auto &ItemReg = EclipseaGameplay::ItemMgr;

    ItemTextureMeta meta00{2038, 8, 8};
    ItemReg.registerItemTextureMeta(meta00);

    ItemInfo bottle_empty{"bottle_empty", 1, 2038, 9, 1.0f, 9};
    ItemInfo bottle_full{"bottle_full", 2, 2038, 10, 1.0f, 9};
    ItemInfo control_basic{"control_basic", 3, 2038, 0, 1.0f, 1};
    ItemInfo control_balanced{"control_balanced", 4, 2038, 1, 1.0f, 1};
    ItemInfo control_advanced{"control_advanced", 5, 2038, 2, 1.0f, 1};
    ItemInfo nuclearcore_basic{"nuclearcore_basic", 6, 2038, 3, 1.0f, 1};
    ItemInfo nuclearcore_balanced{"nuclearcore_balanced", 7, 2038, 4, 1.0f, 1};
    ItemInfo nuclearcore_advanced{"nuclearcore_advanced", 8, 2038, 5, 1.0f, 1};
    ItemInfo storage_basic{"storage_basic", 9, 2038, 6, 1.0f, 1};
    ItemInfo storage_balanced{"storage_balanced", 10, 2038, 7, 1.0f, 1};
    ItemInfo storage_advanced{"storage_advanced", 11, 2038, 8, 1.0f, 1};

    ItemReg.registerItem(bottle_empty);
    ItemReg.registerItem(bottle_full);
    ItemReg.registerItem(control_basic);
    ItemReg.registerItem(control_balanced);
    ItemReg.registerItem(control_advanced);
    ItemReg.registerItem(nuclearcore_basic);
    ItemReg.registerItem(nuclearcore_balanced);
    ItemReg.registerItem(nuclearcore_advanced);
    ItemReg.registerItem(storage_basic);
    ItemReg.registerItem(storage_balanced);
    ItemReg.registerItem(storage_advanced);
#pragma endregion

#pragma region 注册纹理元数据
    auto &TMMGR = EclipseaTextureMetaManager::getInstance();

    EclipseaTextureMeta symbolMeta{2052, 8, 8};
    TMMGR.registerTextureMeta(symbolMeta);
#pragma endregion

    // ── 初始化世界控制器（地图加载 / 实体生成）────────────────
    // 注意：需在实体、物品注册完毕之后调用
    getServerWorldController().onEnter();

    // region === 主循环 ===
    try
    {
        engine.MainLoop();
        engine.CleanUp();
    }
    catch (const std::exception &e)
    {
        LOG("The engine exited with error : {}", e.what());
        return false;
    }
    // endregion

    return true;
}

bool EclipseaApp::CleanUp()
{
    Eclipsea::AudioManager::getInstance().CleanUp();
    return true;
}
