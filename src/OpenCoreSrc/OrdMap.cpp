#include "OpenCore/OpenCore.hpp"

#include <fstream>
#include <stdexcept>
#include <string>

using namespace std;

void OrdMap::Init()
{
    // 初始化地图
    std::fstream mapFile("maps//" + to_string(id) + ".ocm", ios::binary);

    if (!mapFile)
    {
        throw runtime_error("Failed to open a map that not exists!\n");
        return;
    }

    // 读取长宽
    uint8_t Info;
    mapFile.read(reinterpret_cast<char *>(&Info), 1);
    if (!mapFile)
    {
        throw runtime_error(
            "Encountered errors during the info reading stage.\n");
        return;
    }

    mapWidth = (Info >> 4) & 0x0F;
    mapHeight = Info & 0x0F;

    if (mapWidth * mapHeight == 0)
    {
        throw runtime_error("Failed! the map is empty!\n");
        return;
    }
}

void OrdMap::onUpdate(float totalTime)
{
    // 暂且空置
}

void OrdMap::onRender()
{

    // 直接开始以玩家位置为中心绘制地图
}