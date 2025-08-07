#include <iostream>
#include <stdexcept>

// 引入内核
#include "OpenCore/OpenCore.hpp"

using namespace std;

int WinMain(int argc, char** args)
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