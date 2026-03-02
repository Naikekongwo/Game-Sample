#ifndef _STAGE_H_
#define _STAGE_H_

// Stage.hpp
// 场景的基类

#include <SDL2/SDL.h>

#include "OpenCore/Runtime/Graphics/Manager/ElementManager.hpp"

#include <memory>

class ResourceManager;
class SoundEffectManager;
class Timer;
class StageManager;
// 前向定义

enum StageType : uint8_t
{
    baseStage,
    overlayStage,
    topStage,
    unregistered
};

enum stageState
{
    alive,
    pause,
    died
};

class Stage
{
  public:
    virtual ~Stage() = default;
    virtual bool handlEvents(SDL_Event *event) = 0;
    virtual void onEnter() {};
    virtual void onUpdate() = 0;
    virtual void onRender() = 0;
    virtual void onExit() {};
    virtual void onDestroy();

    // 虚函数列表自上而下依次是
    // 析构函数
    // 事件处理函数
    // 场景加入管理器的函数
    // 场景的更新函数
    // 场景的渲染函数
    // 场景退出的函数
    // 场景的销毁函数

    // 场景类型
    StageType getStageType() const { return stageType; }
    void setStageType(StageType stageType) { this->stageType = stageType; }

    // 元素传送相关
    bool transferElementTo(Stage *destStage, const std::string &id);
    bool transferElementFrom(Stage *srcStage, const std::string &id);

    // 临时获取ElementManager
    ElementManager *getElementManager() const { return Elements.get(); }

  protected:
    // 先前内置的渲染器、资源管理器和音效管理器全部都被弃用了
    Timer *timer = nullptr;
    // 场景控制器
    StageManager *sController;
    // 场景类型
    StageType stageType = unregistered;
    // 元素管理器
    unique_ptr<ElementManager> Elements;
};

#endif //_STAGE_H_