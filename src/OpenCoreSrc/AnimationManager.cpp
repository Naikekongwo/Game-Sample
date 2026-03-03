#include "OpenCore/OpenCore.hpp"
#include <algorithm>

void AnimationManager::onUpdate(float totalTime, AnimationState &state)
{
    vector<size_t> eraseList;

    for (size_t i = 0; i < Animations.size(); ++i)
    {
        auto &anime = Animations[i];
        anime->onUpdate(totalTime, state);

        if (!anime->isLoop() && anime->isFinished())
        {
            eraseList.push_back(i);
        }
        else if (sequential)
        {
            // 顺序模式下，一旦遇到未完成动画就跳出，后续动画不执行
            break;
        }
    }

    // 倒序删除已完成动画，避免下标错乱
    for (auto it = eraseList.rbegin(); it != eraseList.rend(); ++it)
    {
        Animations.erase(Animations.begin() + *it);
    }
}

void AnimationManager::pushAnimation(shared_ptr<IAnimation> anime)
{
    Animations.push_back(anime);
}

void AnimationManager::eraseAnimation(shared_ptr<IAnimation> anime)
{
    auto it = std::find(Animations.begin(), Animations.end(), anime);
    if (it != Animations.end())
        Animations.erase(it);
}

void AnimationManager::clear() { Animations.clear(); }

void AnimationManager::setSequence(bool isSequential)
{
    this->sequential = isSequential;
}

void AnimationManager::reset(float totalTime, AnimationState &state)
{
    for (auto entry : Animations)
    {
        entry->reset(totalTime, state);
    }
}