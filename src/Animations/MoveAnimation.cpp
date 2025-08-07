#include "OpenCore/OpenCore.hpp"

// 构造函数实现
MoveAnimation::MoveAnimation(int startX, int startY, int endX, int endY, float duration, bool isLooping)
    : startX(startX), startY(startY), endX(endX), endY(endY), duration(duration), isLooping(isLooping), finished(false) {}

// 更新动画状态
void MoveAnimation::onUpdate(float totalTime, AnimationState& state) {
    if(startTime == 0.0f)
    {
        startTime = totalTime;
        reset(totalTime, state);
    }
    // 如果开始时间为0就重置
    
    if (duration <= 0.0f) {
        SDL_Log("MoveAnimation: duration <= 0, animation failed."); // 持续时间非法
        finished = true;
        return;
    }
    float elapsed = totalTime - startTime; // 计算已用时间
    if (elapsed < 0.0f) elapsed = 0.0f;
    float t = elapsed / duration; // 计算进度
    if (t >= 1.0f) {
        t = 1.0f;
        finished = !isLooping;
        if (isLooping) startTime = totalTime; // 循环则重置起始时间
        SDL_Log("MoveAnimation: animation finished."); // 动画结束
    }
    // 线性插值计算当前位置
    state.scaleX = startX + (endX - startX) * t;
    state.scaleY = startY + (endY - startY) * t;
}

// 判断动画是否结束
bool MoveAnimation::isFinished() const {
    return finished;
}

// 重置动画
void MoveAnimation::reset(float totalTime, AnimationState& state) {
    startTime = totalTime;
    finished = false;
    SDL_Log("MoveAnimation: reset success.");
}