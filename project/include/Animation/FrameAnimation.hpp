// FrameAnimation.hpp
// 帧动画的头文件
#ifndef _FRAMEANIMATION_H_
#define _FRAMEANIMATION_H_

#include "OpenCore/Animation/IAnimation.hpp"
// 帧动画类继承自IAnimation

#include <cstdint>

class FrameAnimation : public IAnimation
{
    public:
    FrameAnimation(uint8_t totalFrames, uint8_t FPS, bool isLooping);
    // 构造方法

    void onUpdate(float totalTime, AnimationState& state) override;
    // 更新动画状态

    bool isFinished() const override;
    // 检查动画是否完成

    void reset(float totalTime, AnimationState& state) override;
    // 重置动画状态

    bool isLoop() override { return isLooping; }
    // 检查动画是否循环

    private:
    float startTime = 0.0f; // 起始时间
    uint8_t currentFrame; // 当前帧索引
    uint8_t totalFrames; // 总帧数
    uint8_t FPS; // 帧率
    
    bool isLooping; // 是否循环
};

#endif //_FRAMEANIMATION_H_