// AnimationManager.h
// 动画管理器的类

#ifndef _ANIMATIONMANAGER_H_
#define _ANIMATIONMANAGER_H_

#include "IAnimation.h"
#include "FrameAnimation.h"
#include "FadeAnimation.h"
#include "MoveAnimation.h"
#include "RotateAnimation.h"
#include "ScaleAnimation.h"

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

private:
    // 改为vector存储
    std::vector<std::shared_ptr<IAnimation>> Animations;
};

#endif //_ANIMATIONMANAGER_H_