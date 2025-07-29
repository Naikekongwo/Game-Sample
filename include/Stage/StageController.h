#ifndef _STAGECONTROLLER_H_
#define _STAGECONTROLLER_H_

// StageController.h
// 场景控制器的头
#include "Stage.h"

#include "PreloadStage.h"

#include <iostream>
#include <vector>

class StageController {
    public:
    StageController() = default;
    ~StageController();

    // 切换场景
    void changeStage(Stage* newStage);

    // 处理事件的公共接口
    bool handlEvents(SDL_Event* event);

    // 更新逻辑
    void onUpdate();
    
    // 渲染逻辑
    void onRender();
    
    private:
    Stage* currentStage = nullptr;
};

#endif //_STAGECONTROLLER_H_