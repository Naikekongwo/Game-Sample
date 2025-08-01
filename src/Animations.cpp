#include "OpenCore/Animation/FrameAnimation.h"

FrameAnimation::FrameAnimation(float StartTime, int totalFrames, int FPS, bool isLooping)
{
    this->AnimationStartTime = StartTime;
    this->totalFrames = totalFrames;
    this->FPS = FPS;
    this->isLooping = isLooping;

    this->currentFrame = 0; // 从第一张开始
}

void FrameAnimation::onUpdate(float totalTime, AnimationState& state)
{
    float elapsedTime = totalTime - AnimationStartTime;
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

void FrameAnimation::reset(float totalTime)
{
    AnimationStartTime = totalTime;
    currentFrame = 0;
}