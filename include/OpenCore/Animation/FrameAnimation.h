// FrameAnimation.h
// 帧动画的头文件
#ifndef _FRAMEANIMATION_H_
#define _FRAMEANIMATION_H_

#include "IAnimation.h"
// 帧动画类继承自IAnimation

class FrameAnimation : public IAnimation
{
    public:
    FrameAnimation(float StartTime, int totalFrames, int FPS, bool isLooping);
    // 构造方法

    void onUpdate(float totalTime, AnimationState& state) override;
    // 更新动画状态

    bool isFinished() const override;
    // 检查动画是否完成

    void reset(float totalTime) override;
    // 重置动画状态

    bool isLoop() override { return isLooping; }
    // 检查动画是否循环

    private:
    float AnimationStartTime = 0.0f;

    int currentFrame; // 当前帧索引
    int totalFrames; // 总帧数
    int FPS; // 帧率
    
    bool isLooping; // 是否循环
};

#endif //_FRAMEANIMATION_H_