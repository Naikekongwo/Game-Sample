
#include "Console.h"

using namespace std;

void ConsoleOut(string text)
{
    cout<<text<<endl;
    cout<<SDL_GetError()<<endl;
}