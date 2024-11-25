#include <iostream>
#include <stdexcept>
#include <SDL2/SDL.h>

using namespace std;

// 定义SDL的类
class SDL_Adapter {
    public:
    bool run(){
        if(!Init()) return EXIT_FAILURE;
        if(!Load()) return EXIT_FAILURE;

        // Blit image to entire window
        SDL_BlitSurface( image1, NULL, winSurface, NULL );

        // Blit image to scaled portion of window
        SDL_Rect dest;
        dest.x = 160;
        dest.y = 120;
        dest.w = 320;
        dest.h = 240;
        SDL_BlitScaled( image2, NULL, winSurface, &dest );

        // Update window
        SDL_UpdateWindowSurface( window );
        system("pause");

        CleanUp();
        return true;
    }
    private:
    SDL_Window* window;
    SDL_Surface* winSurface;
    SDL_Surface* image1;
    SDL_Surface* image2;
    bool Init(){
        if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
        {
            cout<<"Error: Failed to initialize SDL, "<<SDL_GetError()<<endl;
            system("pause");
            return EXIT_FAILURE;
        }

        window = SDL_CreateWindow("Game-Sample",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,1280,720,SDL_WINDOW_SHOWN);
        if(!window)
        {
            cout<<"Error: Failed to create window, "<<SDL_GetError()<<endl;
            system("pause");
            return EXIT_FAILURE;
        }

        winSurface = SDL_GetWindowSurface(window);
        if(!winSurface)
        {
            cout<<"Error: Failed to fetch window's surface, "<<SDL_GetError()<<endl;
            system("pause");
            return EXIT_FAILURE;
        }

        return true;
    }
    bool Load(){
        SDL_Surface *temp1,*temp2;

        // 加载图片
        temp1 = SDL_LoadBMP("visual/LW_STUDIO_LOGO.bmp");
        temp2 = SDL_LoadBMP("visual/LW_STUDIO_LOGO.bmp");

        // 确保图片正确加载
        if(!temp1|!temp2)
        {
            cout<<"Error: Failed to load picture, "<<SDL_GetError()<<endl;
            system("pause");
            return EXIT_FAILURE;
        }

        image1 = SDL_ConvertSurface(temp1,winSurface->format,0);
        image2 = SDL_ConvertSurface(temp2,winSurface->format,0);

        SDL_FreeSurface(temp1);
        SDL_FreeSurface(temp2);

        if(!image1|!image2)
        {
            cout<<"Error: Failed to convert surface to the right form, "<<SDL_GetError()<<endl;
            system("pause");
            return EXIT_FAILURE;
        }
        return true;
    }
    void CleanUp(){
        SDL_FreeSurface(image1);
        SDL_FreeSurface(image2);
        SDL_DestroyWindow(window);
        SDL_Quit();        
    }

    
};

int WinMain(int argc, char** args)
{
    SDL_Adapter SDL;
    try
    {
        SDL.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
        
    return EXIT_SUCCESS;
    // 成功退出后返回 0 
}