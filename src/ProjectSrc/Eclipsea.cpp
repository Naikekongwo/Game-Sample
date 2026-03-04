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
        Console_Log("Engine initialization was failed.");
        return false;
    }

    auto timer = engine.getTimer();
    auto sController = engine.getStageController();

    auto eclipseaInfo = std::make_unique<GameInfo>(
        "Eclipsea", 1, 2, true,
        std::make_unique<PreloadStage>(timer, sController), 1920, 1080, true);

    engine.GameRegistry(std::move(eclipseaInfo));

    // 配置生物

    auto &entityReg = engine.getEntityRegister();

    EntityInfo player{1, player_texture};

    entityReg.registerEntity(player);

    // 生物结束

    try
    {
        engine.MainLoop();
        engine.CleanUp();
    }
    catch (std::exception e)
    {
        Console_Log("The engine exited with error : %s", e.what());
        return false;
    }

    return true;
}