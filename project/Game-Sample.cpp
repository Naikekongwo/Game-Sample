#include <iostream>
#include <stdexcept>

// 引入内核
#include "OpenCore/OpenCore.hpp"

using namespace std;

// #define TESTBENCH_MODE // 打开测试模式的宏

// 测试平台入口
#ifdef TESTBENCH_MODE
int main(int argc, char *argv[])
{
    // 测试平台的代码写于此 
    return EXIT_SUCCESS;
}
#else
// 游戏入口
int main(int argc, char *argv[])
{
    OpenEngine engine;
    
    try
    {
        engine.Run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return EXIT_SUCCESS;
}
#endif