// AnimationManager.hpp
// 动画管理器的类

#ifndef _ANIMATIONMANAGER_H_
#define _ANIMATIONMANAGER_H_

#include "OpenCore/Animation/IAnimation.hpp"

#include <cstdint>
#include <memory>
#include <vector>

// 枚举类
enum class AnimationID { FrameAnime, FadeAnime, MoveAnimation, RotateAnimation, ScaleAnimation};

struct Texture
{
    // 行数和列数
    uint8_t xCount = 1;
    uint8_t yCount = 1;

    // 贴图的大小
    uint16_t width,height;

    // 材质
    std::shared_ptr<OpenCore_Tilemap> texture;

    Texture(uint8_t x, uint8_t y, std::shared_ptr<OpenCore_Tilemap> tex);

    uint16_t Size() const noexcept { return xCount * yCount; }

    OpenCore_Rect getSrcRect(uint8_t index);    
};

class AnimationManager
{
public:
    void onUpdate(float totalTime, AnimationState& state); // 刷新函数

    void pushAnimation(std::shared_ptr<IAnimation> anime); // 只需传动画指针

    void eraseAnimation(std::shared_ptr<IAnimation> anime); // 可选：按指针移除动画

    bool isEmpty() const { return Animations.empty(); }

    void clear(); // 清空动画表

    bool hasAnimation() const { return !Animations.empty(); } // 判断是否存在

    void setSequence(bool isSequential);

    bool isFinished() const { 
        // 所有动画都完成才算 finished
        if (Animations.empty()) return true;
        for (const auto& anime : Animations) {
            if (!anime->isFinished()) return false;
        }
        return true;
    }

private:
    // 改为vector存储
    std::vector<std::shared_ptr<IAnimation>> Animations;

    // 是否顺序执行 ： 默认为 false 即默认是并行执行
    bool sequential = false;
};

#endif //_ANIMATIONMANAGER_H_