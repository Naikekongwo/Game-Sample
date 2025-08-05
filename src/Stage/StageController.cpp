// StageController.cpp
// 场景控制器的具体实现
// 以及场景类的部分实现

#include "OpenCore/OpenCore.h"

void StageController::changeStage(std::unique_ptr<Stage> newStage)
{
    // [SHAOYANG] 2025/7/29
    // 已经全部换用unique_ptr来管理对象
    currentStage = std::move(newStage);
}

bool StageController::handlEvents(SDL_Event *event)
{
    return (currentStage) ? currentStage->handlEvents(event) : false;
}

void StageController::onUpdate()
{
    if (currentStage)
        currentStage->onUpdate();
}

void StageController::onRender()
{
    if (currentStage)
        currentStage->onRender();
}