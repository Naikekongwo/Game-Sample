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
    // 当前帧索引
    int frameIndex = 0; 
    
    // 不透明度，默认为1.0f（完全不透明）
    float transparency = 1.0f;

    // X和Y方向的缩放比例，二者默认为100%
    float scaleX = 1.0f;
    float scaleY = 1.0f;

    // 动画开始时间
    float AnimeStartTime = 0.0f; 

    // 位置参量，默认为0,0
    int PositionX = 0;
    int PositionY = 0;
    // 锚定位置，默认为5（中心对齐）
    int Anchor = 5;

    // 对齐位置的定义如下:
    // [1][2][3]
    // [4][5][6]
    // [7][8][9]
};

class IAnimation
{
    public:
    virtual ~IAnimation() = default;

    virtual void onUpdate(float totalTime, AnimationState& state) = 0;
    // virtual void onRender(SDL_Renderer* renderer) = 0;
    // 弃用！！！！ 不要在动画内部实现渲染方法！
    virtual bool isFinished() const = 0;
    virtual void reset(float totalTime, AnimationState& state) = 0;

    virtual bool isLoop() = 0;
};

#endif //_IANIMATION_H_