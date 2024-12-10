// _GRAPHICS_

#include "Graphics.h"
#include "Macro.h"
#include "Console.h"
#include <stdexcept>

bool SDL_Adapter::run()
{
    if(!Init()) return EXIT_FAILURE;
    if(!Load()) return EXIT_FAILURE;
    // 初始化 和 加载素材

    int textureSize = 500;
    float scale = 0.5;

    // 加载手柄
    int joyStickCount = SDL_NumJoysticks();
    for(int i = 0; i < joyStickCount;i++) {
        // 遍历所有手柄设备并注册
        if(SDL_IsGameController(i)) {
            //这里的i是索引，而不是地址
            SDL_GameController *controller = SDL_GameControllerOpen(i);
            if(controller) {
                std::cout<<"Opened controller, index: "<<i<<", name: "<<SDL_GameControllerName(controller);
            } else {
                std::cout<<"Could not open controller, index: "<<i<<", which lacks GameController API supports. "<<SDL_GetError();
            }
        }
    }
    
    SDL_Rect Center;
    Center.x = WINDOW_WIDTH/2-textureSize*scale*0.5;
    Center.y = WINDOW_HEIGHT/2-textureSize*scale*0.5;
    Center.w = textureSize * scale;
    Center.h = textureSize * scale;

    SDL_BlitScaled(studioLOGO, NULL, winSurface, &Center);
    SDL_UpdateWindowSurface(window);

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
            
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_w:
                    Center.y--;
                    SDL_BlitScaled(studioLOGO, NULL, winSurface, &Center);
                    SDL_UpdateWindowSurface(window);
                    break;
                case SDLK_s:
                    Center.y++;
                    SDL_BlitScaled(studioLOGO, NULL, winSurface, &Center);
                    SDL_UpdateWindowSurface(window);
                    break;
                case SDLK_a:
                    Center.x--;
                    SDL_BlitScaled(studioLOGO, NULL, winSurface, &Center);
                    SDL_UpdateWindowSurface(window);
                    break;
                case SDLK_d:
                    Center.x++;
                    SDL_BlitScaled(studioLOGO, NULL, winSurface, &Center);
                    SDL_UpdateWindowSurface(window);
                    break;
                }
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

bool SDL_Adapter::Init()
{
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        // 初始化SDL不成功
        ConsoleOut("SDL初始化失败。");
        return EXIT_FAILURE;        
    }
    // 初始化结束

    window = SDL_CreateWindow(GAME_TITLE,SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,WINDOW_WIDTH,WINDOW_HEIGHT,SDL_WINDOW_SHOWN);
    if(!window)
    {
        // 窗口创建失败
        ConsoleOut("窗口创建失败");
        return EXIT_FAILURE;
    }
    // 窗口创建结束

    winSurface = SDL_GetWindowSurface(window);
    if(!winSurface)
    {
        ConsoleOut("Surface获取失败。");
        return EXIT_FAILURE;
    }

    return true;
}

bool SDL_Adapter::Load()
{
    SDL_Surface *temp1;
        // 创建图片缓存

        temp1 = SDL_LoadBMP("visual/LW_STUDIO_LOGO.bmp");

        if(!temp1)
        {
            ConsoleOut("图片加载失败。");
            system("pause");
            return EXIT_FAILURE;
        }

        studioLOGO = SDL_ConvertSurface(temp1,winSurface->format,0);

        SDL_FreeSurface(temp1);

        if(!studioLOGO)
        {
            ConsoleOut("图片转换失败。");
            system("pause");
            return EXIT_FAILURE;
        }

        return true;
    return true;
}

void SDL_Adapter::CleanUp()
{
    SDL_FreeSurface(studioLOGO);
    SDL_DestroyWindow(window);
    SDL_Quit();
}