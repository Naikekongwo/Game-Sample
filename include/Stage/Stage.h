#ifndef _STAGE_H_
#define _STAGE_H_

// Stage.h
// 场景的基类

#include <SDL2/SDL.h>

class Stage{
    public:
    Stage(SDL_Renderer* render) : renderer(render) {};
    ~Stage() = default;

    // 虚函数部分

    // 初始化逻辑
    virtual bool handlEvents(SDL_Event* event) = 0;

    // 更新逻辑
    virtual void onUpdate() = 0;

    // 渲染逻辑
    virtual void onRender() = 0;


    protected:
    SDL_Renderer* renderer = nullptr;

};

#endif //_STAGE_H_