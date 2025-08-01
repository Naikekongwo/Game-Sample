// IAnimation.h
// 动画的基类头文件
#ifndef _IANIMATION_H_
#define _IANIMATION_H_

#include <SDL2/SDL.h>

// AnimationState 动画状态
// 他会返回一系列的状态信息
// 例如透明度、尺寸、或者是位置等
struct AnimationState
{
    int frameIndex = 0; // 当前帧索引
    float transparency = 1.0f; // 透明度
    float scaleX = 1.0f;       // X轴缩放
    float scaleY = 1.0f; // Y轴缩放
};

class IAnimation
{
    public:
    virtual ~IAnimation() = default;

    virtual void onUpdate(float totalTime, AnimationState& state) = 0;
    // virtual void onRender(SDL_Renderer* renderer) = 0;
    // 弃用！！！！ 不要在动画内部实现渲染方法！
    virtual bool isFinished() const = 0;
    virtual void reset(float totalTime) = 0;

    virtual bool isLoop() = 0;
};

#endif //_IANIMATION_H_