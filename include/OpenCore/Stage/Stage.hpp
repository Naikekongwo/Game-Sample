#ifndef _STAGE_H_
#define _STAGE_H_

// Stage.hpp
// 场景的基类

#include <SDL2/SDL.h>

class ResourceManager;
class SoundEffectManager;
class Timer; 
class StageController;
// 前向定义

enum StageType { baseStage, overlayStage, topStage, unregistered};

class Stage{
    public:
    virtual ~Stage()
    {
        onDestroy();
    }

    // 虚函数部分

    // 初始化逻辑
    virtual bool handlEvents(SDL_Event* event) = 0;

    // 更新逻辑
    virtual void onUpdate() = 0;

    // 渲染逻辑
    virtual void onRender() = 0;

    // 回收逻辑
    virtual void onDestroy();

    // 场景类型
    StageType getStageType() const { return stageType; }

    // 元素传送相关
    bool transferElementTo(Stage* destStage, const std::string& id);
    bool transferElementFrom(Stage* srcStage, const std::string& id);

    // 临时获取ElementManager
    ElementManager* getElementManager() const { return Elements.get(); }

    protected:
    // 先前内置的渲染器、资源管理器和音效管理器全部都被弃用了
    Timer* timer = nullptr;
    // 场景控制器
    StageController* sController;
    // 场景类型
    StageType stageType = unregistered;
    // 元素管理器
    std::unique_ptr<ElementManager> Elements;
};

#endif //_STAGE_H_