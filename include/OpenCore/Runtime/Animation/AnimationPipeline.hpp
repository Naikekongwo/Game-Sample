#pragma once

// AnimationPipeline.hpp
// 这是一个Pipeline的模板类
// 我们在实际游戏中引用的是一个叫做AnimationPipeline的类，继承自本类

#include "OpenCore/Runtime/Animation/Concrete/CollectionAnimation.hpp"
#include "OpenCore/Runtime/Animation/Concrete/FadeAnimation.hpp"
#include "OpenCore/Runtime/Animation/Concrete/FrameAnimation.hpp"
#include "OpenCore/Runtime/Animation/Concrete/MoveAnimation.hpp"
#include "OpenCore/Runtime/Animation/Concrete/RotateAnimation.hpp"
#include "OpenCore/Runtime/Animation/Concrete/ScaleAnimation.hpp"
#include "OpenCore/Runtime/Animation/Concrete/TimerAnimation.hpp"

#include "OpenCore/Runtime/Animation/Manager/AnimationManager.hpp"

#include <cstdint>
#include <memory>
#include <vector>

using std::shared_ptr;
using std::vector;

class AnimationPipeline
{
  public:
    AnimationPipeline(AnimationManager *AnimeManager,
                      AnimationPipeline *parentPipeline)
        : manager(AnimeManager), parentPipeline(parentPipeline) {};

    // 添加帧动画
    AnimationPipeline &Frame(uint8_t totalFrames, uint8_t FPS,
                             bool isLooping = false)
    {
        animations.push_back(
            std::make_shared<FrameAnimation>(totalFrames, FPS, isLooping));
        return *this;
    }
    // 添加渐变动画
    AnimationPipeline &Fade(float startAlpha, float endAlpha, float duration,
                            bool isLooping = false)
    {
        animations.push_back(std::make_shared<FadeAnimation>(
            startAlpha, endAlpha, duration, isLooping));
        return *this;
    }
    // 添加移动动画
    AnimationPipeline &Move(uint16_t startX, uint16_t startY, uint16_t endX,
                            uint16_t endY, float duration,
                            bool isLooping = false)
    {
        animations.push_back(std::make_shared<MoveAnimation>(
            startX, startY, endX, endY, duration, isLooping));
        return *this;
    }
    // 添加缩放动画
    AnimationPipeline &Scale(float startScale, float endScale, float duration,
                             bool isLooping = false)
    {
        animations.push_back(std::make_shared<ScaleAnimation>(
            startScale, endScale, duration, isLooping));
        return *this;
    }
    // 添加旋转动画
    AnimationPipeline &Rotate(float startAngle, float endAngle, float duration,
                              bool isLooping = false)
    {
        animations.push_back(std::make_shared<RotateAnimation>(
            startAngle, endAngle, duration, isLooping));
        return *this;
    }
    // 添加定时器动画
    AnimationPipeline &Timer(float duration)
    {
        animations.push_back(std::make_shared<TimerAnimation>(duration));
        return *this;
    }

    AnimationPipeline *SubStart(bool isParalle)
    {
        auto colani = std::make_shared<CollectionAnimation>(this, isParalle);
        animations.push_back(colani);
        // colani 是集合的共享指针
        return colani->Begin();
    }

    AnimationPipeline &SubEnd()
    {
        Commit();
        // 将当前子段的动画全部返回
        return *parentPipeline;
    }

    // 执行，将所有动画加入管理器
    void Commit()
    {
        for (auto &anime : animations)
        {
            manager->pushAnimation(anime);
        }
        animations.clear();
    }

  protected:
    AnimationPipeline *parentPipeline = nullptr;
    AnimationManager *manager = nullptr;       // 动画管理器指针
    vector<shared_ptr<IAnimation>> animations; // 存储动画的容器
};