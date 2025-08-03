#include "OpenCore/OpenCore.h"

FrameAnimation::FrameAnimation(int totalFrames, int FPS, bool isLooping)
{
    this->totalFrames = totalFrames;
    this->FPS = FPS;
    this->isLooping = isLooping;

    this->currentFrame = 0; // 从第一张开始
}

void FrameAnimation::onUpdate(float totalTime, AnimationState& state)
{
    if(startTime == 0.0f)
    {
        startTime = totalTime;
        reset(totalTime, state);
    }
    // 如果开始时间为0就重置
    
    float elapsedTime = totalTime - startTime;
    if (elapsedTime < 0.0f) elapsedTime = 0.0f;

    int frame = static_cast<int>(elapsedTime * FPS);

    if (isLooping) {
        frame = frame % totalFrames;  // 循环时用取模
    } else {
        if (frame >= totalFrames) {
            frame = totalFrames - 1;   // 非循环动画卡在最后一帧
        }
    }

    currentFrame = frame;
    state.frameIndex = currentFrame;
}


bool FrameAnimation::isFinished() const
{
    return (!isLooping && currentFrame >= totalFrames - 1);
}

void FrameAnimation::reset(float totalTime, AnimationState& state)
{
    startTime = totalTime;
    currentFrame = 0;
}
