// IDrawable.h
// 可绘图类的基类
#ifndef _IDRAWABLE_H_
#define _IDRAWABLE_H_

#include <SDL2/SDL.h>

#include <memory>

enum class AnchorPoint : int;

class IDrawableObject
{
    public:

    // 处理事件的函数
    virtual void handlEvents(SDL_Event &event, float totalTime) = 0;

    // 更新函数
    virtual void onUpdate(float totalTime) = 0;

    // 渲染函数
    virtual void onRender(SDL_Renderer* render) = 0;

    // 获取碰撞箱
    virtual SDL_Rect getBounds() = 0;

    // 获取 ID 和 图层的方法
    short getID() const { return id; }
    short getLayer() const { return layer; }

    void setScale(int w, int h) { bWidth = w;bHeight = h;}
    void setAnchor(AnchorPoint anchor) { AnimeState->Anchor = anchor; }

    void setPosition(int x, int y) { AnimeState->PositionX = x; AnimeState->PositionY = y;}

    protected:
    // 默认id
    short id = 0;
    // 默认图层
    short layer = 0;
    // 动画管理器
    std::unique_ptr<AnimationManager> AnimeManager;
    // 动画状态
    std::unique_ptr<AnimationState> AnimeState;
    // 元素基础大小
    int bWidth, bHeight;
    // 贴图
    std::unique_ptr<Texture> texture;
};

#endif //_IDRAWABLE_H_