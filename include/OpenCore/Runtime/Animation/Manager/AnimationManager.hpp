// AnimationManager.hpp
// 动画管理器的类

#ifndef _ANIMATIONMANAGER_H_
#define _ANIMATIONMANAGER_H_

#include "OpenCore/Runtime/Animation/IAnimation.hpp"

#include "SDL2/SDL.h"

#include <cstdint>
#include <memory>
#include <vector>

using std::shared_ptr;
using std::vector;

struct Texture
{
    // 行数和列数
    size_t xCount = 1;
    size_t yCount = 1;

    // 贴图的大小
    uint16_t width, height;

    // 材质
    shared_ptr<SDL_Texture> texture;

    Texture(size_t x, size_t y, shared_ptr<SDL_Texture> tex);

    uint16_t Size() const noexcept { return xCount * yCount; }

    float getWidthHeight() const noexcept
    {
        return static_cast<float>(width) / static_cast<float>(height);
    }

    SDL_Rect getSubRect(size_t index);
    SDL_Texture *get() const { return texture.get(); }
};

class AnimationManager
{
  public:
    void onUpdate(float totalTime, AnimationState &state); // 刷新函数

    void pushAnimation(shared_ptr<IAnimation> anime); // 只需传动画指针

    void eraseAnimation(shared_ptr<IAnimation> anime); // 可选：按指针移除动画

    bool isEmpty() const { return Animations.empty(); }

    void clear(); // 清空动画表

    void reset(float totalTime, AnimationState &state); // 重置所有动画

    bool hasAnimation() const { return !Animations.empty(); } // 判断是否存在

    void setSequence(bool isSequential);

    bool isFinished() const
    {
        // 所有动画都完成才算 finished
        if (Animations.empty())
            return true;
        for (const auto &anime : Animations)
        {
            if (!anime->isFinished())
                return false;
        }
        return true;
    }

  private:
    // 改为vector存储
    vector<shared_ptr<IAnimation>> Animations;

    // 是否顺序执行 ： 默认为 false 即默认是并行执行
    bool sequential = false;
};

#endif //_ANIMATIONMANAGER_H_