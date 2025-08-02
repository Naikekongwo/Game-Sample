#pragma once
#include "IAnimation.h"

// 缩放动画类
class ScaleAnimation : public IAnimation {
public:
    // 构造函数，传入起始和结束缩放、持续时间、是否循环
    ScaleAnimation(float startScale, float endScale, float duration, bool isLooping);

    void onUpdate(float totalTime, AnimationState& state) override; // 更新动画
    bool isFinished() const override; // 是否结束
    void reset(float totalTime, AnimationState& state) override; // 重置
    bool isLoop() override { return isLooping; } // 是否循环

private:
    float startScale, endScale, duration; // 起始、结束缩放和持续时间
    bool isLooping; // 是否循环
    bool finished;  // 是否结束
};