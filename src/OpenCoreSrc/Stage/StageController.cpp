#include "OpenCore/OpenCore.hpp"
#include "Eclipsea/Eclipsea.hpp"

void StageController::changeBaseStage(std::unique_ptr<BaseStage> newStage)
{   
    stageContainer[0] = std::move(newStage);
}

void StageController::changeOverlayStage(std::unique_ptr<OverlayStage> newStage)
{   
    stageContainer[1] = std::move(newStage);
}

void StageController::changeTopStage(std::unique_ptr<TopStage> newStage)
{   
    stageContainer[2] = std::move(newStage);
}

void StageController::changeStage(std::unique_ptr<Stage> newStage)
{
    if (!newStage) return;
    
    switch (newStage->getStageType()) {
        case baseStage:
            stageContainer[0] = std::move(newStage);
            break;
        case overlayStage:
            stageContainer[1] = std::move(newStage);
            break;
        case topStage:
            stageContainer[2] = std::move(newStage);
            break;
        default:
            break;
    }
}

bool StageController::handlEvents(SDL_Event* event)
{
    // 从后往前处理：top -> overlay -> base
    for (int i = 2; i >= 0; --i) {
        if (stageContainer[i] && stageContainer[i]->handlEvents(event)) {
            return true;
        }
    }
    return false;
}

void StageController::onUpdate()
{
    for (auto& stage : stageContainer) {
        if (stage) {
            stage->onUpdate();
        }
    }
}

void StageController::onRender()
{
    for (auto& stage : stageContainer) {
        if (stage) {
            stage->onRender();
        }
    }
}