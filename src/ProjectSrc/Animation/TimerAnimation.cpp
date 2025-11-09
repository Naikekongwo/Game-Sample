#include "OpenCore/OpenCore.hpp"
#include "Eclipsea/Eclipsea.hpp"

TimerAnimation::TimerAnimation(float duration)
{
    this->duration = duration;
    // 设置持续时间
    this->finished = false;
}

void TimerAnimation::onUpdate(float totalTime, AnimationState& state)
{
    if(startTime == 0.0f) startTime = totalTime;

    if(totalTime - startTime >= duration)
    {
        // 超时
        finished = true;
    }
}

void TimerAnimation::reset(float totalTime, AnimationState& state)
{
    startTime = totalTime;
}

bool TimerAnimation::isFinished() const
{
    return finished;
}