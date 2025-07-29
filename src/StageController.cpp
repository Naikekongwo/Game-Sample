// StageController.cpp
// 场景控制器的具体实现
// 以及场景类的部分实现

#include "Stage/StageController.h"

StageController::~StageController()
{
    // 资源回收机制
    // 如果当前场景仍未得到销毁，即销毁
    if(currentStage) {
        delete currentStage;
        currentStage = nullptr;
    }
}


void StageController::changeStage(Stage* newStage)
{
    delete currentStage; // 删除旧的场景
    currentStage = newStage; // 切换到新的场景
}

bool StageController::handlEvents(SDL_Event* event)
{
    return currentStage->handlEvents(event);
}

void StageController::onUpdate()
{
    currentStage->onUpdate();
}

void StageController::onRender()
{
    currentStage->onRender();
}