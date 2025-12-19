// IAnimation.hpp
// 动画的基类头文件
#ifndef _IANIMATION_H_
#define _IANIMATION_H_

#include <SDL2/SDL.h>
#include <atomic>
#include <string>

#include "OpenCore/Graphics/Renderer/IRenderer.hpp"

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
    // 帧容量
    // 帧引索
    uint8_t frameCapacity = 0;
    uint8_t frameIndex = 0;

    // 不透明度
    float transparency = 1.0f;
    // 尺寸缩放（ 0 X 1 Y ）
    float scale[2] {1.0f, 1.0f};

    // 角度
    float angle = 0.0f;

    // 位置
    int16_t Position[2] {0,0};

    // 锚点
    AnchorPoint Anchor = AnchorPoint::Center;    
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