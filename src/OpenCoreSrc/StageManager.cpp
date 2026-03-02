
#include "OpenCore/OpenCore.hpp"
#include <cstddef>

void StageManager::changeStage(unique_ptr<Stage> newStage)
{
    if (!newStage)
    {
        Console_Log("StageManager::changeStage encountered a nulllptr in "
                    "pushing stage.");
        return;
    }
    commandQueue.push(std::make_unique<StageCommand>(
        StageCommandType::Add, newStage->getStageType(), std::move(newStage)));
}

void StageManager::removeStage(StageType sType)
{
    auto index = static_cast<size_t>(sType);

    if (index >= kStageCount)
        return;

    if (!stageContainer[index])
        return;

    commandQueue.push(std::make_unique<StageCommand>(StageCommandType::Remove,
                                                     sType, nullptr));
}

void StageManager::onUpdate()
{
    processCommandQueue();
    // 更新函数
    for (auto &stage : stageContainer)
    {
        if (!stage)
            continue;

        stage->onUpdate();
    }
}

void StageManager::processCommandQueue()
{
    bool output = !commandQueue.empty();

    if (output)
        Console_Log("processCommandQueue start, queue size = %d",
                    (int)commandQueue.size());

    while (!commandQueue.empty())
    {
        auto task = std::move(commandQueue.front());
        commandQueue.pop();

        size_t index = static_cast<size_t>(task->sType);

        if (index >= kStageCount)
            continue;

        switch (task->opt)
        {
        case StageCommandType::Add:
            Console_Log("Adding stage to index %zu", index);
            if (stageContainer[index])
            {
                stageContainer[index]->onExit();
                stageContainer[index]->onDestroy();
            }
            stageContainer[index] = std::move(task->stage_);
            stageContainer[index]->onEnter();
            // 完整的执行替换前的场景的回收函数
            // 然后完整的执行新场景的初始化内容
            break;

        case StageCommandType::Remove:
            Console_Log("Removing stage at index %zu", index);
            if (stageContainer[index])
            {
                stageContainer[index]->onExit();
                stageContainer[index]->onDestroy();
                stageContainer[index].reset();
                // 完整的销毁场景
            }
            break;

        default:
            break;
        }
    }

    if (output)
        Console_Log("processCommandQueue end, queue size = %d",
                    (int)commandQueue.size());
}

void StageManager::onRender()
{
    // 渲染
    for (auto &stage : stageContainer)
    {
        if (!stage)
            continue;

        stage->onRender();
    }
}

bool StageManager::handlEvents(SDL_Event *event)
{
    for (auto it = stageContainer.rbegin(); it != stageContainer.rend(); ++it)
    {
        if (*it)
        {
            (*it)->handlEvents(event);
            break;
        }
    }
    return true;
}