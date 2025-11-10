#ifndef _IDRAWABLE_H_
#define _IDRAWABLE_H_

// IDrawableObject.hpp
// 可绘图类的基类

#include <SDL2/SDL.h>
#include <string>
#include <memory>

class AnimationManager;
class AnimationPipeline;
class MyAnimationPipeline;
class DrawableConfigurator;

// 如你所见，上方我们给了MyAnimationPipeline一个前向声明
// 但是你知道，这个类我们在引擎内部没有给出其定义
// 这是一个作为接口存在的类
// 我们默认游戏会实现自己的MyAnimationPipeline！

struct Texture;

struct AnimationState;

enum class AnchorPoint : int;

class IDrawableObject
{
public:
    // 默认的构造方法
    IDrawableObject();

    // 处理事件的函数
    virtual void handlEvents(SDL_Event &event, float totalTime);

    // 更新函数
    // 2025/10/19 给予默认的方法，进行动画更新
    virtual void onUpdate(float totalTime);

    // 渲染函数
    virtual void onRender() = 0;

    // 自毁函数
    virtual bool onDestroy() = 0;

    // 获取逻辑碰撞箱
    virtual SDL_Rect getBounds() = 0;

    // 获取实际碰撞箱
    virtual SDL_Rect getRenderedBounds() = 0;


    // 更改贴图
    void changeTexture(std::unique_ptr<Texture> newTexture);

    // 设置动画顺序
    void setSequential(bool sequential);

    // 获取 ID 和 图层的方法
    std::string getID() const { return id; }
    short getLayer() const { return layer; }
    bool isAnimeFinished() const;

    // 属性设置的方法
    void setScale(float w, float h);
    
    void setAnchor(AnchorPoint anchor);
    void setPosition(float xPercent, float yPercent);

    void setParentContainer(IDrawableObject* parentContainer);

    // 配置器方法
    MyAnimationPipeline Animate();
    DrawableConfigurator Configure();

protected:
    // ID 已经弃用，改用string代替
    std::string id;
    // 默认图层
    uint8_t layer = 0;
    // 动画管理器
    std::unique_ptr<AnimationManager> AnimeManager;
    // 动画状态
    std::unique_ptr<AnimationState> AnimeState;
    // 元素基础大小
    uint16_t nativeWidth, nativeHeight;
    // 贴图
    std::unique_ptr<Texture> texture;
    // 父容器类
    bool absolutePosite = true;
    IDrawableObject* parentContainer = nullptr;
};

#endif //_IDRAWABLE_H_