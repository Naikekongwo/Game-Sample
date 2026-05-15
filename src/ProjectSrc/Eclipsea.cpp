#include "Eclipsea/Eclipsea.hpp"
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

    auto timer = engine.getTimer();
    auto sController = engine.getStageController();

    auto eclipseaInfo = std::make_unique<GameInfo>(
        "Eclipsea", 1, 8, true,
        std::make_unique<PreloadStage>(timer, sController), 1920, 1080, true);

    engine.GameRegistry(std::move(eclipseaInfo));

    // 配置生物

    auto &entityReg = engine.getEntityRegister();

    EntityInfo player{PLAYER_ENTITYTYPE, player_texture, 8, 1.0f};

    entityReg.registerEntity(player);

    auto &entityReg1 = engine.getEntityRegister();

    EntityInfo player1{2, chao_texture, 8, 1.0f};

    entityReg.registerEntity(player1);

    EntityInfo purifier{100, 2044, 4, 3.0f, 3.0f, 1, 0, 0};

    entityReg.registerEntity(purifier);

    // 生物结束

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

    return true;
}