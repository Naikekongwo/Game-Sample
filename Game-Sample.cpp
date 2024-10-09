// Game-Sample.cpp
// 本游戏的入口源代码处。

#include <iostream>
#include <stdexcept>
#include <SDL2/SDL.h>

using namespace std;

int main(int argc,char** args)
{
    system("pause");
    // 按照惯例来个你好世界。
    return EXIT_SUCCESS;
    // EXIT_SUCCESS 实际上就是 0，该宏被定义在 stdexcept 头文件中。
}