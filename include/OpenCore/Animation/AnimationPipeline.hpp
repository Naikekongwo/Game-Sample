#pragma once


#include "AnimationManager.hpp"

#include <cstdint>
#include <memory>
#include <vector>

class AnimationPipeline {
public:
    // 构造函数(弃用)
    AnimationPipeline(AnimationManager* mgr) : manager(mgr) {}

    // 添加帧动画
    AnimationPipeline& Frame(uint8_t totalFrames, uint8_t FPS, bool isLooping = false) {
        animations.push_back(std::make_shared<FrameAnimation>(totalFrames, FPS, isLooping));
        return *this;
    }
    // 添加渐变动画
    AnimationPipeline& Fade(float startAlpha, float endAlpha, float duration, bool isLooping = false) {
        animations.push_back(std::make_shared<FadeAnimation>(startAlpha, endAlpha, duration, isLooping));
        return *this;
    }
    // 添加移动动画
    AnimationPipeline& Move(uint16_t startX, uint16_t startY, uint16_t endX, uint16_t endY, float duration, bool isLooping = false) {
        animations.push_back(std::make_shared<MoveAnimation>(startX, startY, endX, endY, duration, isLooping));
        return *this;
    }
    // 添加缩放动画
    AnimationPipeline& Scale(float startScale, float endScale, float duration, bool isLooping = false) {
        animations.push_back(std::make_shared<ScaleAnimation>(startScale, endScale, duration, isLooping));
        return *this;
    }
    // 添加旋转动画
    AnimationPipeline& Rotate(float startAngle, float endAngle, float duration, bool isLooping = false) {
        animations.push_back(std::make_shared<RotateAnimation>(startAngle, endAngle, duration, isLooping));
        return *this;
    }
    // 添加定时器动画
    AnimationPipeline& Timer(float duration) {
        animations.push_back(std::make_shared<TimerAnimation>(duration));
        return *this;
    }

    // 执行，将所有动画加入管理器
    void Commit() {
        for (auto& anime : animations) {
            manager->pushAnimation(anime);
        }
        animations.clear();
    }

private:
    AnimationManager* manager = nullptr;  // 动画管理器指针
    std::vector<std::shared_ptr<IAnimation>> animations;    // 存储动画的容器
};