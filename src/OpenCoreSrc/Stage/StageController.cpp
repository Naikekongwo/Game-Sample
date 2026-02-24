#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"


void StageController::changeStage(unique_ptr<Stage> newStage)
{
    if (!newStage)
    {
        SDL_Log("StageController::changeStage encountered a nulllptr in "
                "pushing stage.");
        return;
    }
    StreamLine.push(std::make_unique<OperateRecord>(
        Operation::Add, newStage->getStageType(), std::move(newStage)));
}

void StageController::removeStage(StageType sType)
{
    int index = static_cast<int>(sType);

    if (index < 0 || index >= 3)
        return;

    if (stageContainer[index]) // ✅ 修正
    {
        StreamLine.push(
            std::make_unique<OperateRecord>(Operation::Remove, sType, nullptr));
    }
}

void StageController::onUpdate()
{
    // 更新函数
    for (auto &entry : stageContainer)
    {
        if (entry)
        {
            entry->onUpdate();
        }
    }

    ParsingStreamLine();
}

void StageController::ParsingStreamLine()
{
    bool output = ((int)StreamLine.size() == 0) ? false : true;
    // 处理流水线操作的
    if (output)
        SDL_Log("ParsingStreamLine start, queue size = %d",
                (int)StreamLine.size());

    while (!StreamLine.empty())
    {
        // 当这个序列不为空
        auto task = std::move(StreamLine.front());
        switch (task->opt)
        {
        case Operation::Add:
        {
            SDL_Log("sController::StreamLine Now we are adding one stage to %d",
                    (int)task->sType);
            // 进行新增的操作
            switch (task->sType)
            {
            case baseStage:
                stageContainer[0] = std::move(task->stage_);
                break;
            case overlayStage:
                stageContainer[1] = std::move(task->stage_);
                break;
            case topStage:
                stageContainer[2] = std::move(task->stage_);
                break;
            default:
                continue;
            }
            break;
        }
        case Operation::Remove:
        {
            SDL_Log(
                "sController::StreamLine Now we are removing one stage to %d",
                (int)task->sType);
            switch (task->sType)
            {
            case baseStage:
                stageContainer[0].reset();
                break;
            case overlayStage:
                stageContainer[1].reset();
                break;
            case topStage:
                stageContainer[2].reset();
                break;
            default:
                continue;
            }
            break;
        }
        default:
            break;
        }
        StreamLine.pop();
    }
    if (output)
        SDL_Log("ParsingStreamLine end, queue size = %d",
                (int)StreamLine.size());
}

void StageController::onRender()
{
    // 渲染
    for (auto &entry : stageContainer)
    {
        if (entry)
        {
            entry->onRender();
        }
    }
}

bool StageController::handlEvents(SDL_Event *event)
{
    for (auto &entry : stageContainer)
    {
        if (entry)
        {
            entry->handlEvents(event);
        }
    }
    return true;
}