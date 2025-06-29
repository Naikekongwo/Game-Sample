#ifndef _GAME_PRELOAD_H
#define _GAME_PRELOAD_H

#include "../macro.h"

#include "stage.h"
#include "../engine/graphics/atlas.h"
#include "../engine/graphics/modules.h"

#include "main_menu.h"

class game_preload: public stage
{
    private:
    stageController* sController;
    SDL_Renderer* renderer;
    SDL_Window* window;

    Timer* timer; // 该计时器为只读计时器，不应该在内部被修改

    Atlas* atlas; // 纹理图集

    int currentIndex = 0; // 当前索引


    public:
    game_preload(stageController* sCon); // 构造函数
    ~game_preload();


    bool onUpdate() override; // 更新函数
    bool onRender() override; // 渲染函数
    bool handleEvent(SDL_Event &event) override; // 事件函数

    Timer* getTime() {return timer;} // 获取定时器
};

#endif // _GAME_PRELOAD_H