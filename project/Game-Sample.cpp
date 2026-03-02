#include <SDL2/SDL_error.h>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

// 引入内核
#include "Eclipsea/Eclipsea.hpp"

#include <cmath>
const int WIDTH = 1920, HEIGHT = 1080;

using namespace std;

// 游戏入口
int main(int argc, char *argv[])
{
    Eclipsea game;

    if (!game.StartUp())
    {
        throw runtime_error("The game has exited with a bad return-value!");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
