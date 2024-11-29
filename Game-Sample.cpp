#include <iostream>
#include <stdexcept>
#include <SDL2/SDL.h>

using namespace std;

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

// 定义SDL的类
class SDL_Adapter {
    public:
    bool run(){
        if(!Init()) return EXIT_FAILURE;
        if(!Load()) return EXIT_FAILURE;

        int Texture = 500;
        float scale = 1.0;
        SDL_Rect Center;
        Center.x = WINDOW_WIDTH/2-Texture * scale*0.5;
        Center.y = WINDOW_HEIGHT/2-Texture * scale*0.5;
        Center.w = Texture * scale;
        Center.h = Texture * scale;

        // Blit image to the right portion
        SDL_BlitScaled( image1, NULL, winSurface, &Center);

        // Update window
        SDL_UpdateWindowSurface( window );
        
        // 事件刷新
        bool should_quit = false;
        SDL_Event event;

        while(!should_quit)
        {
            while(SDL_PollEvent(&event))
            {
                switch ((event.type))
                {
                case SDL_QUIT:
                    should_quit = true;
                    break;
                
                default:
                    break;
                }
            }
            SDL_Delay(100);
        }

        CleanUp();
        return true;
    }
    private:
    SDL_Window* window;
    SDL_Surface* winSurface;
    SDL_Surface* image1;
    bool Init(){
        if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
        {
            cout<<"Error: Failed to initialize SDL, "<<SDL_GetError()<<endl;
            system("pause");
            return EXIT_FAILURE;
        }

        window = SDL_CreateWindow("Game-Sample",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,WINDOW_WIDTH,WINDOW_HEIGHT,SDL_WINDOW_SHOWN);
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
        SDL_Surface *temp1;

        // 加载图片
        temp1 = SDL_LoadBMP("visual/LW_STUDIO_LOGO.bmp");

        // 确保图片正确加载
        if(!temp1)
        {
            cout<<"Error: Failed to load picture, "<<SDL_GetError()<<endl;
            system("pause");
            return EXIT_FAILURE;
        }

        image1 = SDL_ConvertSurface(temp1,winSurface->format,0);

        SDL_FreeSurface(temp1);

        if(!image1)
        {
            cout<<"Error: Failed to convert surface to the right form, "<<SDL_GetError()<<endl;
            system("pause");
            return EXIT_FAILURE;
        }
        return true;
    }
    void CleanUp(){
        SDL_FreeSurface(image1);
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