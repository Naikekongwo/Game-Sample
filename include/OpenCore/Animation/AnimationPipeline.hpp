#pragma once

// AnimationPipeline.hpp
// 这是一个Pipeline的模板类
// 我们在实际游戏中引用的是一个叫做MyAnimationPipeline的类，继承自本类

#include "OpenCore/Animation/Manager/AnimationManager.hpp"

#include <cstdint>
#include <memory>
#include <vector>

class AnimationPipeline {
public:
    // 构造函数(弃用)
    AnimationPipeline(AnimationManager* mgr) : manager(mgr) {}

    // 添加帧动画 : 是一个接口的实例
    // AnimationPipeline& Frame(uint8_t totalFrames, uint8_t FPS, bool isLooping = false) {
    //     animations.push_back(std::make_shared<FrameAnimation>(totalFrames, FPS, isLooping));
    //     return *this;
    // }

    // 执行，将所有动画加入管理器
    void Commit() {
        for (auto& anime : animations) {
            manager->pushAnimation(anime);
        }
        animations.clear();
    }

protected:
    AnimationManager* manager = nullptr;  // 动画管理器指针
    std::vector<std::shared_ptr<IAnimation>> animations;    // 存储动画的容器
};