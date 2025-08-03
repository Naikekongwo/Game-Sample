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

    int Size() const { return xCount * yCount; }

    SDL_Rect getSrcRect(int index);    
};

class AnimationManager
{
    public:
    void onUpdate(float totalTime, AnimationState& state); // 刷新函数

    void pushAnimation(int id, std::shared_ptr<IAnimation> anime);
    
    void eraseAnimation(int id);

    bool isEmpty() const { return Animations.empty(); }

    private:
    // 内部动画表
    std::unordered_map<int, std::shared_ptr<IAnimation>> Animations;
    
};

#endif //_ANIMATIONMANAGER_H_