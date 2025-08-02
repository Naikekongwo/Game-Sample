#include "OpenCore/Animation/ScaleAnimation.h"
#include <SDL2/SDL_log.h>

// 构造函数实现
ScaleAnimation::ScaleAnimation(float startScale, float endScale, float duration, bool isLooping)
    : startScale(startScale), endScale(endScale), duration(duration), isLooping(isLooping), finished(false) {}

// 更新动画状态
void ScaleAnimation::onUpdate(float totalTime, AnimationState& state) {
    if (duration <= 0.0f) {
        SDL_Log("ScaleAnimation: duration <= 0, animation failed."); // 持续时间非法
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
        SDL_Log("ScaleAnimation: animation finished."); // 动画结束
    }
    // 线性插值计算当前缩放
    state.scaleX = state.scaleY = startScale + (endScale - startScale) * t;
    SDL_Log("ScaleAnimation: onUpdate success, t=%.2f, scale=%.2f", t, state.scaleX);
}

// 判断动画是否结束
bool ScaleAnimation::isFinished() const {
    return finished;
}

// 重置动画
void ScaleAnimation::reset(float totalTime, AnimationState& state) {
    state.AnimeStartTime = totalTime;
    finished = false;
    SDL_Log("ScaleAnimation: reset success.");
}