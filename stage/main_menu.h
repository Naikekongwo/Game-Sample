#ifndef _MAIN_MENU_H_
#define _MAIN_MENU_H_

#include "../macro.h"

#include "stage.h"
#include "../engine/graphics/atlas.h"
#include "../engine/graphics/modules.h"

// 主页面场景

class main_stage : public stage
{
    private:
    stageController* sController;
    SDL_Renderer* renderer;
    SDL_Window* window;

    Timer* timer;

    Atlas* atlas;

    public:
    main_stage(stageController* sCon); // 构造函数

    bool onUpdate() override; // 更新函数
    bool onRender() override; // 渲染函数
    bool handleEvent(SDL_Event &event) override; // 事件函数

    Timer* getTime() {return sController->getTime();} // 获取定时器
};


#endif // _MAIN_MENU_H_