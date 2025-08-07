// AnimationManager.hpp
// 动画管理器的类

#ifndef _ANIMATIONMANAGER_H_
#define _ANIMATIONMANAGER_H_

#include "IAnimation.hpp"
#include "FrameAnimation.hpp"
#include "FadeAnimation.hpp"
#include "MoveAnimation.hpp"
#include "RotateAnimation.hpp"
#include "ScaleAnimation.hpp"
#include "TimerAnimation.hpp"

#include <memory>
#include <vector>

// 枚举类
enum class AnimationID { FrameAnime, FadeAnime, MoveAnimation, RotateAnimation, ScaleAnimation};

struct Texture
{
    // 行数和列数
    int xCount = 1;
    int yCount = 1;

    // 贴图的大小
    int width,height;

    // 材质
    SDL_Texture* texture;

    Texture(int x, int y, SDL_Texture* tex);

    int Size() const noexcept { return xCount * yCount; }

    SDL_Rect getSrcRect(int index);    
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