#pragma once
#include "IAnimation.hpp"

// 旋转动画类
class RotateAnimation : public IAnimation {
public:
    // 构造函数，传入起始和结束角度、持续时间、是否循环
    RotateAnimation(float startAngle, float endAngle, float duration, bool isLooping);

    void onUpdate(float totalTime, AnimationState& state) override; // 更新动画
    bool isFinished() const override; // 是否结束
    void reset(float totalTime, AnimationState& state) override; // 重置
    bool isLoop() override { return isLooping; } // 是否循环

private:
    float startTime = 0.0f; // 起始时间
    float startAngle, endAngle, duration; // 起始、结束角度和持续时间
    bool isLooping; // 是否循环
    bool finished;  // 是否结束
};