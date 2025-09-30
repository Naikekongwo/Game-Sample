#ifndef _IDRAWABLE_H_
#define _IDRAWABLE_H_

// IDrawableObject.hpp
// 可绘图类的基类

#include <SDL2/SDL.h>
#include <memory>

class AnimationManager;
class AnimationPipeline;
class DrawableConfigurator;

struct Texture;

struct AnimationState;

enum class AnchorPoint : int;

class IDrawableObject
{
public:
    // 处理事件的函数
    virtual void handlEvents(SDL_Event &event, float totalTime) = 0;

    // 更新函数
    virtual void onUpdate(float totalTime) = 0;

    // 渲染函数
    virtual void onRender() = 0;

    // 获取碰撞箱
    virtual SDL_Rect getBounds() = 0;

    // 更改贴图
    void changeTexture(Texture *newTexture);

    // 设置动画顺序
    void setSequential(bool sequential);

    // 获取 ID 和 图层的方法
    std::string getID() const { return id; }
    short getLayer() const { return layer; }
    bool isAnimeFinished() const;

    // 属性设置的方法
    void setScale(int w, int h)
    {
        bWidth = w;
        bHeight = h;
    }
    void setAnchor(AnchorPoint anchor);
    void setPosition(int x, int y);

    // 配置器方法
    AnimationPipeline Animate();
    DrawableConfigurator Configure();

protected:
    // ID 已经弃用，改用string代替
    std::string id;
    // 默认图层
    short layer = 0;
    // 动画管理器
    std::unique_ptr<AnimationManager> AnimeManager;
    // 动画状态
    std::unique_ptr<AnimationState> AnimeState;
    // 元素基础大小
    int bWidth, bHeight;
    // 贴图
    Texture *texture;
};

#endif //_IDRAWABLE_H_