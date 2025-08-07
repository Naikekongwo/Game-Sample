#ifndef _STAGE_H_
#define _STAGE_H_

// Stage.hpp
// 场景的基类

#include <SDL2/SDL.h>

class ResourceManager;
class SoundEffectManager;
class Timer; 
// 前向定义

class Stage{
    public:
    ~Stage() = default;

    // 虚函数部分

    // 初始化逻辑
    virtual bool handlEvents(SDL_Event* event) = 0;

    // 更新逻辑
    virtual void onUpdate() = 0;

    // 渲染逻辑
    virtual void onRender() = 0;


    protected:
    // 先前内置的渲染器、资源管理器和音效管理器全部都被弃用了
    Timer* timer = nullptr;
};

#endif //_STAGE_H_