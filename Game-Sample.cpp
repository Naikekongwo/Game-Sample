#include <iostream>
#include <stdexcept>

#include "opencore.h"

using namespace std;


int WinMain(int argc, char** args)
{
    openCore core;
    try
    {
        core.Start();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        system("pause");
    }
    
    return EXIT_SUCCESS;
}