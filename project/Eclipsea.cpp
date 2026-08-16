#include <cstdlib>
#include <iostream>

// 引入内核
#include "Eclipsea/Eclipsea.hpp"

#include <cmath>
const int WIDTH = 1920, HEIGHT = 1080;

// 游戏入口
int main(int argc, char *argv[])
{
    EclipseaApp game;

    if (!game.StartUp())
    {
        LOG("游戏启动失败，请检查日志以获取详细信息。");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
