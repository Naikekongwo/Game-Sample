// IAnimation.h
// 动画的基类头文件
#ifndef _IANIMATION_H_
#define _IANIMATION_H_

#include <SDL2/SDL.h>
#include <atomic>
#include <string>

// AnimationState 动画状态
// 他会返回一系列的状态信息
// 例如透明度、尺寸、或者是位置等
enum class AnchorPoint : int {
    TopLeft = 1, TopCenter, TopRight,
    MiddleLeft, Center, MiddleRight,
    BottomLeft, BottomCenter, BottomRight
};

struct AnimationState
{
    int frameIndex = 0;           // 当前帧索引
    float transparency = 1.0f;    // 透明度
    float scaleX = 1.0f;          // X轴缩放
    float scaleY = 1.0f;          // Y轴缩放
    float angle = 0.0f;           // 角度

    AnchorPoint Anchor = AnchorPoint::Center; // 锚点 默认居中
    int PositionX = 0; // X坐标
    int PositionY = 0; // Y坐标
};

class IAnimation
{
public:
   
    virtual ~IAnimation() = default;


    // 设置/获取动画名称
    void setName(const std::string& n) { name = n; }
    const std::string& getName() const { return name; }

    virtual void onUpdate(float totalTime, AnimationState& state) = 0;
    virtual bool isFinished() const = 0;
    virtual void reset(float totalTime, AnimationState& state) = 0;
    virtual bool isLoop() = 0;

private:
    std::string name;             // 动画名称
 
};



#endif //_IANIMATION_H_