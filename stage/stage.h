#ifndef _STAGE_H
#define _STAGE_H

// stage.h 场景基类

#include <SDL2/SDL.h>
#include <vector>
#include "../engine/Timer.h"

class stage
{
    public:
    virtual ~stage(){} // 虚析构函数

    virtual bool onUpdate() = 0; // 更新函数
    virtual bool onRender() = 0; // 渲染函数
    virtual bool handleEvent(SDL_Event &event) = 0; // 事件函数
};

class stageController
{
    private:
    std::vector<stage*> stages; // 场景列表
    SDL_Renderer* renderer; // 渲染器
    SDL_Window* window; // 窗口

    Timer* timer; // 定时器

    public:

    stageController(SDL_Renderer* r, SDL_Window* w, Timer* t) : renderer(r), window(w),timer(t){} // 构造函数

    SDL_Renderer* getRenderer(){return renderer;} // 获取渲染器
    SDL_Window* getWindow(){return window;} // 获取窗口

    void pushStage(stage* s){stages.push_back(s);} // 添加场景
    bool popStage(){return (isEmpty())?false:(stages.back()->~stage(), stages.pop_back(), true);} // 删除场景
    bool changeState(stage* s){return (isEmpty())?false:(stages.back()->~stage(), stages.pop_back(), stages.push_back(s), true);} // 替换场景

    Timer* getTime() {return timer;} // 获取定时器

    bool isEmpty(){return stages.empty();} // 判断场景列表是否为空
    bool onUpdate(){return (isEmpty())?false:stages.back()->onUpdate();} // 更新函数
    bool onRender(){return (isEmpty())?false:stages.back()->onRender();} // 渲染函数
    bool handleEvent(SDL_Event &event){return (isEmpty())?false:stages.back()->handleEvent(event);} // 事件函数
};

#endif // _STAGE_H