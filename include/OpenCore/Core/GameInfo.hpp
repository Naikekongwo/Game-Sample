#pragma once

// GameInfo.hpp
// 游戏的元数据

#include <memory>
#include <string>

using std::string;
using std::unique_ptr;

class Stage;

struct GameInfo
{
    string gameName = "openengine-game";
    uint8_t version_major = 1;
    uint8_t version_minor = 0;
    bool beta = false;

    unique_ptr<Stage> entranceStage;
};