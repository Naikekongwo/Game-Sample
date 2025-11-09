#pragma once

// MyAnimationPipeline.hpp
// “无水之境所需要的渲染管线”

#include "OpenCore/Animation/AnimationPipeline.hpp"

#include "FadeAnimation.hpp"
#include "FrameAnimation.hpp"
#include "MoveAnimation.hpp"
#include "RotateAnimation.hpp"
#include "ScaleAnimation.hpp"
#include "TimerAnimation.hpp"

class MyAnimationPipeline : public AnimationPipeline
{
    public:
    // 添加帧动画
    MyAnimationPipeline& Frame(uint8_t totalFrames, uint8_t FPS, bool isLooping = false) {
        animations.push_back(std::make_shared<FrameAnimation>(totalFrames, FPS, isLooping));
        return *this;
    }
    // 添加渐变动画
    MyAnimationPipeline& Fade(float startAlpha, float endAlpha, float duration, bool isLooping = false) {
        animations.push_back(std::make_shared<FadeAnimation>(startAlpha, endAlpha, duration, isLooping));
        return *this;
    }
    // 添加移动动画
    MyAnimationPipeline& Move(uint16_t startX, uint16_t startY, uint16_t endX, uint16_t endY, float duration, bool isLooping = false) {
        animations.push_back(std::make_shared<MoveAnimation>(startX, startY, endX, endY, duration, isLooping));
        return *this;
    }
    // 添加缩放动画
    MyAnimationPipeline& Scale(float startScale, float endScale, float duration, bool isLooping = false) {
        animations.push_back(std::make_shared<ScaleAnimation>(startScale, endScale, duration, isLooping));
        return *this;
    }
    // 添加旋转动画
    MyAnimationPipeline& Rotate(float startAngle, float endAngle, float duration, bool isLooping = false) {
        animations.push_back(std::make_shared<RotateAnimation>(startAngle, endAngle, duration, isLooping));
        return *this;
    }
    // 添加定时器动画
    MyAnimationPipeline& Timer(float duration) {
        animations.push_back(std::make_shared<TimerAnimation>(duration));
        return *this;
    }
};