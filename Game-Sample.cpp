#include <iostream>
#include "Graphics.h"
#include <stdexcept>

using namespace std;


int main(int argc,char** args)
{
    SDL_Adapter sdlAdaper;
    try
    {
        sdlAdaper.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}