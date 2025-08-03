#pragma once
#include "IAnimation.h"

// 位置移动动画类
class MoveAnimation : public IAnimation {
public:
    // 构造函数，传入起始和结束坐标、持续时间、是否循环
    MoveAnimation(int startX, int startY, int endX, int endY, float duration, bool isLooping);

    // 更新动画状态
    void onUpdate(float totalTime, AnimationState& state) override;
    // 判断动画是否结束
    bool isFinished() const override;
    // 重置动画
    void reset(float totalTime, AnimationState& state) override;
    // 是否循环
    bool isLoop() override { return isLooping; }

private:
    float startTime = 0.0f; // 起始时间
    int startX, startY, endX, endY; // 起始和结束坐标
    float duration;                 // 持续时间
    bool isLooping;                 // 是否循环
    bool finished;                  // 是否结束
};