#ifndef _STAGECONTROLLER_H_
#define _STAGECONTROLLER_H_

// StageController.hpp
// 场景控制器的头
#include "Stage.hpp"
#include "PreloadStage.hpp"
#include "MainStage.hpp"
#include "GameplayStage.hpp"

#include <memory>
#include <iostream>
#include <vector>

class StageController {
    public:
    ~StageController() = default;

    // 切换场景
    void changeStage(std::unique_ptr<Stage> newStage);

    // 处理事件的公共接口
    bool handlEvents(SDL_Event* event);

    // 更新逻辑
    void onUpdate();
    
    // 渲染逻辑
    void onRender();
    
    private:
    std::unique_ptr<Stage> currentStage; // 当前场景
};

#endif //_STAGECONTROLLER_H_