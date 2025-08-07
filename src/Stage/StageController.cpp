// StageController.cpp
// 场景控制器的具体实现
// 以及场景类的部分实现

#include "OpenCore/OpenCore.hpp"

void StageController::changeStage(std::unique_ptr<Stage> newStage)
{
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