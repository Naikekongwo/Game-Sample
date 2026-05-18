#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Gameplay/Backpack/ItemManager.hpp"
#include <exception>
#include <memory>
#include <stdexcept>
#include <system_error>

bool Eclipsea::StartUp()
{
    (void)OpenEngine::getInstance();

    OpenEngine &engine = OpenEngine::getInstance();

    if (!engine.Initialize())
    {
        LOG("Engine initialization was failed.");
        return false;
    }

#pragma region 注册游戏信息

    auto timer = engine.getTimer();
    auto sController = engine.getStageController();

    auto eclipseaInfo = std::make_unique<GameInfo>(
        "Eclipsea", 1, 8, true,
        std::make_unique<PreloadStage>(timer, sController), 1920, 1080, true);

    engine.GameRegistry(std::move(eclipseaInfo));
#pragma endregion

#pragma region 注册实体
    auto &entityReg = engine.getEntityRegister();

    EntityInfo player{PLAYER_ENTITYTYPE, {player_texture, 4, 4}, 8, 1.0f};
    entityReg.registerEntity(player);

    EntityInfo player1{2, {chao_texture, 4, 4}, 8, 1.0f};
    entityReg.registerEntity(player1);

    EntityInfo purifier{100, {2044, 8, 8, 0, 1}, 4, 3.0f, 3.0f};
    entityReg.registerEntity(purifier);
#pragma endregion

#pragma region 注册物品相关内容

    auto &ItemReg = Gameplay::ItemMgr;

    ItemTextureMeta meta00{2038, 8, 8};
    ItemReg.registerItemTextureMeta(meta00);

    ItemInfo bottle_empty{"bottle_empty", 1, 2038, 9, 1.0f, 9};
    ItemInfo bottle_full{"bottle_full", 2, 2038, 10, 1.0f, 9};
    ItemReg.registerItem(bottle_empty);
    ItemReg.registerItem(bottle_full);
#pragma endregion

    // region === 主循环 ===
    try
    {
        engine.MainLoop();
        engine.CleanUp();
    }
    catch (std::exception e)
    {
        LOG("The engine exited with error : {}", e.what());
        return false;
    }
    // endregion

    return true;
}