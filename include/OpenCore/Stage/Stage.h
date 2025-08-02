#ifndef _STAGE_H_
#define _STAGE_H_

// Stage.h
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
    SDL_Renderer* renderer = nullptr; //渲染器
    ResourceManager* resourceManager = nullptr; // 资源管理器
    SoundEffectManager* sfxManager = nullptr; // 音效管理器
    Timer* timer = nullptr;
};

#endif //_STAGE_H_