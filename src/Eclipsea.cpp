#include "Eclipsea/Eclipsea.hpp"
#include "Core/Info/ResourceInfo.hpp"
#include "Eclipsea/Core/AudioManager.hpp"
#include "Eclipsea/Core/EclipseaTextures.hpp"
#include "Eclipsea/Gameplay/Backpack/ItemManager.hpp"
#include "Eclipsea/Gameplay/Backpack/Iteminfo.hpp"
#include "Eclipsea/Gameplay/Entity/EntityRegister.hpp"
#include "Eclipsea/Gameplay/Gameplay.hpp"
#include "Eclipsea/Gameplay/TextureMeta.hpp"
#include <exception>
#include <memory>
#include <stdexcept>
#include <system_error>

// 注册全部资源（纹理 / 字体）——沿用 Aozora-Re 的 PackageManager 注册方式。
// 新版 OpenCore 已放弃 script 清单 + 数字 ID，统一以字符串资源名注册。
//
// 注意：引擎的 registerResources 只登记资源清单，并不会登记纹理的网格元信息。
// 而 getTextureObject(name)（BaseBackground / CheckBox / Scrollbar / Symbol 等
// 按名查找的控件）依赖已注册的元信息，否则返回空纹理。因此这里在注册资源的
// 同时，为每个纹理注册其网格元信息（cols × rows）。
static void RegisterAllResources(PackageManager *package)
{
    struct TextureDef
    {
        const char *name;
        const char *path;
        uint8_t     cols;
        uint8_t     rows;
    };

    // 纹理注册表（名称、路径、网格）。图集按实际帧布局填写，其余为 1×1。
    const TextureDef kTextures[] = {
        // ── 1×1 UI / 背景 ──
        {"preload_title", "assets/ui/Preload_Title.png", 1, 1},
        {"icon_opencore", "assets/ui/icon_opencore.png", 1, 1},
        {"icon_studio", "assets/ui/icon_studio.png", 1, 1},
        {"img_connector", "assets/ui/preload_menu_connector.png", 1, 1},
        {"background_main", "assets/backgrounds/main_background.png", 1, 1},
        {"background_gameplay", "assets/backgrounds/gameplay_background.png", 1,
         1},
        {"background_purifier", "assets/ui/purifier_background.png", 1, 1},
        {"main_title", "assets/ui/Title_Main.png", 1, 1},
        {"copyright", "assets/ui/icon_copyright.png", 1, 1},
        {"item_purifier", "assets/ui/purifier/purifier.png", 1, 1},
        {"desset_top", "assets/backgrounds/dessert_top.png", 1, 1},
        {"base_sky", "assets/backgrounds/base_sky.png", 1, 1},
        {"cities_top", "assets/backgrounds/cities_top.png", 1, 1},
        {"button_border", "assets/ui/rect_border.png", 1, 1},
        {"stone_background", "assets/ui/stone_background.png", 1, 1},
        {"img_createworld", "assets/ui/main/Button_Create.png", 1, 1},
        {"img_scrollbutton", "assets/ui/scrollbutton.png", 1, 1},
        {"img_itemcontain", "assets/ui/item_container.png", 1, 1},
        {"pad_copyright", "assets/ui/pad_copyright.png", 1, 1},
        {"effects_explosion", "assets/effects/explosion.png", 1, 1},
        {"story_plane", "assets/story/img_plane.png", 1, 1},
        {"story_ruinedcity", "assets/story/img_ruinedcity.png", 1, 1},
        {"story_starsky", "assets/story/img_starsky.png", 1, 1},
        {"background_typewriter", "assets/ui/background_typewriter.png", 1, 1},
        {"img_insidehouse", "assets/story/img_insidehouse.png", 1, 1},
        {"entity_shadow", "assets/entity/entity_shadow.png", 1, 1},
        {"ui_bg_purifier", "assets/ui/purifier/purifier_background.png", 1, 1},
        {"water", "assets/terrain/water.png", 1, 1},

        // ── 3 帧按钮（normal / pressed / …）──
        {"img_StartButton", "assets/ui/main/Button_Start.png", 1, 3},
        {"img_ContButton", "assets/ui/main/Button_Continue.png", 1, 3},
        {"img_SettButton", "assets/ui/main/Button_Settings.png", 1, 3},
        {"img_BackButton", "assets/ui/settings/Button_Back.png", 1, 3},
        {"btn_backmain", "assets/ui/pause/btn_backmain.png", 1, 3},
        {"btn_pause_settings", "assets/ui/pause/btn_settings.png", 1, 3},

        // ── 两态复选框（未勾选 / 勾选，竖排）──
        {"img_checkbox", "assets/ui/checkbox_classic.png", 1, 2},

        // ── 动画图集 ──
        {"water_drops", "assets/ui/animation_waterdrops.png", 1, 5},
        {"water_waves", "assets/ui/animation_waterwaves.png", 1, 5},
        {"effects_water_bubbles", "assets/effects/waterparticles.png", 5, 9},
        {"effects_flames", "assets/effects/flames.png", 11, 11},
        {"ui_img_healthbar", "assets/ui/gameplay/healthbar.png", 1, 4},

        // ── 实体 / 物品 / 地形 ──
        {"player_texture", "assets/entity/player_texture.png", 4, 4},
        {"chao_texture", "assets/entity/chao_texture.png", 4, 4},
        {"items_sets00", "assets/items/items_sets00.png", 8, 8},
        {"terrain_terrain", "assets/terrain/terrain.png", 8, 8},
        {"symbols", "assets/ui/symbols.png", 8, 8},
    };

    for (const auto &tex : kTextures)
    {
        package->registerResource(RscTexture, tex.name, tex.path);
        package->registerTextureMeta(TextureMeta{tex.name, tex.cols, tex.rows});
    }

    // ── 字体 ──
    package->registerResources({
        {RscFont, "9001", "assets/ui/font/OpenCoreFont.ttf"},
        {RscFont, "9002", "assets/ui/font/OpenCoreCHFont.ttf"},
        {RscFont, "OpenCoreFont", "assets/ui/font/OpenCoreFont.ttf"},
    });
}

// 注册音频（SDL3_mixer 项目级模块，以资源名注册）
static void RegisterAllAudio()
{
    auto &audio = Eclipsea::AudioManager::getInstance();
    audio.registerMusic("music_oceanwaves", "assets/audio/bgm_oceanwaves.mp3");
    audio.registerSound("se_waterdrops", "assets/audio/fx_waterdrops.mp3");
    audio.registerMusic("music_lostworld", "assets/audio/bgm_lostworld.mp3");
    audio.registerMusic("music_someoneinthedark",
                        "assets/audio/bgm_someoneinthedark.mp3");
}

bool EclipseaApp::StartUp()
{
    (void)OpenEngine::getInstance();

    OpenEngine &engine = OpenEngine::getInstance();

    // ── 图形信息 ─────────────────────────────────────────────
    GraphicsInfo gInfo;
    gInfo.keepRatio        = true;
    gInfo.nearestScaling   = true;
    gInfo.resizable        = false;
    gInfo.targetFrameRate  = 144;
    gInfo.resolutionWidth  = 1920;
    gInfo.resolutionHeight = 1080;

    // ── 资源信息 ─────────────────────────────────────────────
    ResourceInfo rInfo;
    rInfo.keepStructureWhenPackaging = true;
    rInfo.packageOnly                = false;
    rInfo.textureResampleFactor      = 1.0f;

    auto eclipseaInfo = std::make_unique<GameInfo>();

    eclipseaInfo->gameName      = "Eclipsea";
    eclipseaInfo->beta          = true;
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

    // ── 注册全部资源（纹理 / 字体）───────────────────────────
    RegisterAllResources(Package);

    // ── 注册并初始化音频 ─────────────────────────────────────
    RegisterAllAudio();
    Eclipsea::AudioManager::getInstance().Init();

#pragma region 注册实体
    auto &entityReg = EntityRegister::getInstance();

    EntityInfo player = EntityInfo::makeCharacter(
        PLAYER_ENTITYTYPE, "player_texture", 8, true, 1.0f);
    entityReg.registerEntity(player);

    EntityInfo player1 =
        EntityInfo::makeCharacter(2, "chao_texture", 8, true, 1.0f);
    entityReg.registerEntity(player1);

    EntityInfo purifier{100, {{"terrain_terrain", 8, 8}, 0, 1}, {}, 4, 0, 3.0f,
                        3.0f};
    entityReg.registerEntity(purifier);

    EntityInfo storage{101, {{"terrain_terrain", 8, 8}, 0, 4}, {}, 4, 0, 2.0f,
                       2.0f};
    entityReg.registerEntity(storage);
#pragma endregion

#pragma region 注册物品相关内容
    auto &ItemReg = EclipseaGameplay::ItemMgr;

    ItemTextureMeta meta00{"items_sets00", 8, 8};
    ItemReg.registerItemTextureMeta(meta00);

    ItemInfo bottle_empty{"bottle_empty", 1, "items_sets00", 9, 1.0f, 9};
    ItemInfo bottle_full{"bottle_full", 2, "items_sets00", 10, 1.0f, 9};
    ItemInfo control_basic{"control_basic", 3, "items_sets00", 0, 1.0f, 1};
    ItemInfo control_balanced{
        "control_balanced", 4, "items_sets00", 1, 1.0f, 1};
    ItemInfo control_advanced{
        "control_advanced", 5, "items_sets00", 2, 1.0f, 1};
    ItemInfo nuclearcore_basic{
        "nuclearcore_basic", 6, "items_sets00", 3, 1.0f, 1};
    ItemInfo nuclearcore_balanced{
        "nuclearcore_balanced", 7, "items_sets00", 4, 1.0f, 1};
    ItemInfo nuclearcore_advanced{
        "nuclearcore_advanced", 8, "items_sets00", 5, 1.0f, 1};
    ItemInfo storage_basic{"storage_basic", 9, "items_sets00", 6, 1.0f, 1};
    ItemInfo storage_balanced{
        "storage_balanced", 10, "items_sets00", 7, 1.0f, 1};
    ItemInfo storage_advanced{
        "storage_advanced", 11, "items_sets00", 8, 1.0f, 1};

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

    EclipseaTextureMeta symbolMeta{"symbols", 8, 8};
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
