#include "OpenCore/Animation/RotateAnimation.h"
#include <SDL2/SDL_log.h>

// 构造函数实现
RotateAnimation::RotateAnimation(float startAngle, float endAngle, float duration, bool isLooping)
    : startAngle(startAngle), endAngle(endAngle), duration(duration), isLooping(isLooping), finished(false) {}

// 更新动画状态
void RotateAnimation::onUpdate(float totalTime, AnimationState& state) {
    if (duration <= 0.0f) {
        SDL_Log("RotateAnimation: duration <= 0, animation failed."); // 持续时间非法
        finished = true;
        return;
    }
    float elapsed = totalTime - state.AnimeStartTime; // 计算已用时间
    if (elapsed < 0.0f) elapsed = 0.0f;
    float t = elapsed / duration; // 计算进度
    if (t >= 1.0f) {
        t = 1.0f;
        finished = !isLooping;
        if (isLooping) state.AnimeStartTime = totalTime; // 循环则重置起始时间
        SDL_Log("RotateAnimation: animation finished."); // 动画结束
    }
    // 线性插值计算当前角度，直接写入angle字段
    state.angle = startAngle + (endAngle - startAngle) * t;
    SDL_Log("RotateAnimation: onUpdate success, t=%.2f, angle=%.2f", t, state.angle);
}

// 判断动画是否结束
bool RotateAnimation::isFinished() const {
    return finished;
}

// 重置动画
void RotateAnimation::reset(float totalTime, AnimationState& state) {
    state.AnimeStartTime = totalTime;
    finished = false;
    SDL_Log("RotateAnimation: reset success.");
}