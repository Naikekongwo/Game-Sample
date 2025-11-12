#include "OpenCore/OpenCore.hpp"
#include "Eclipsea/Eclipsea.hpp"



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
    
    // 更新所有 Stage
    for (auto& stage : stageContainer) {
        if (stage) {
            stage->onUpdate();
        }
    }

    // 执行延迟操作
    while(!StreamLine.empty())
    {
        OperateRecord op = std::move(StreamLine.front());
        StageType stageType = op.second->getStageType();
        if(op.first==Add)
        {
            // 添加场景
            addStage(std::move(op.second));
        }
        else if(op.first==Remove)
        {
            deleteStage(std::move(op.second));
        }
        StreamLine.pop();        
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

void StageController::removeStage(std::unique_ptr<Stage> newStage)
{
    StreamLine.push(std::make_pair(Remove, std::move(newStage)));
}

void StageController::changeStage(std::unique_ptr<Stage> newStage)
{
    StreamLine.push(std::make_pair(Add, std::move(newStage)));
}

void StageController::deleteStage(std::unique_ptr<Stage> newStage)
{
    if(!newStage) return;

    if(newStage->getStageType() == baseStage) stageContainer[0].reset();
    else if(newStage->getStageType() == overlayStage) stageContainer[1].reset();
    else if(newStage->getStageType() == topStage) stageContainer[2].reset();
}

void StageController::addStage(std::unique_ptr<Stage> newStage)
{
    if(!newStage) return;

    if(newStage->getStageType() == baseStage) stageContainer[0] = std::move(newStage);
    else if(newStage->getStageType() == overlayStage) stageContainer[1] = std::move(newStage);
    else if(newStage->getStageType() == topStage) stageContainer[2] = std::move(newStage);
}