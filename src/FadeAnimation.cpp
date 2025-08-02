#include "OpenCore/Animation/FadeAnimation.h"
#include <SDL2/SDL_log.h>

// 构造函数实现
FadeAnimation::FadeAnimation(float startAlpha, float endAlpha, float duration, bool isLooping)
    : startAlpha(startAlpha), endAlpha(endAlpha), duration(duration), isLooping(isLooping), finished(false) {}

// 更新动画状态
void FadeAnimation::onUpdate(float totalTime, AnimationState& state) {
    if (duration <= 0.0f) {
        SDL_Log("FadeAnimation: duration <= 0, animation failed."); // 持续时间非法
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
        SDL_Log("FadeAnimation: animation finished."); // 动画结束
    }
    // 线性插值计算当前透明度
    state.transparency = startAlpha + (endAlpha - startAlpha) * t;
    SDL_Log("FadeAnimation: onUpdate success, t=%.2f, alpha=%.2f", t, state.transparency);
}

// 判断动画是否结束
bool FadeAnimation::isFinished() const {
    return finished;
}

// 重置动画
void FadeAnimation::reset(float totalTime, AnimationState& state) {
    state.AnimeStartTime = totalTime;
    finished = false;
    SDL_Log("FadeAnimation: reset success.");
}