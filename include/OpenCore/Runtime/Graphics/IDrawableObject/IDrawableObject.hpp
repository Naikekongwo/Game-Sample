#ifndef _IDRAWABLE_H_
#define _IDRAWABLE_H_

// IDrawableObject.hpp
// 可绘图类的基类

#include <SDL2/SDL.h>
#include <memory>
#include <string>

using std::string;
using std::unique_ptr;

class AnimationManager;
class AnimationPipeline;
class AnimationPipeline;
class DrawableConfigurator;

// 如你所见，上方我们给了AnimationPipeline一个前向声明
// 但是你知道，这个类我们在引擎内部没有给出其定义
// 这是一个作为接口存在的类
// 我们默认游戏会实现自己的AnimationPipeline！

struct Texture;

struct VisualState;

enum class AnchorPoint : int;

class IDrawableObject
{
  public:
    // 默认的构造方法
    IDrawableObject();

    virtual ~IDrawableObject() = default;

    // 处理事件的函数
    virtual void handlEvents(SDL_Event &event, float totalTime);
    virtual void onUpdate(float totalTime);
    /**
     * @brief 非原生渲染方法
     * @deprecated 由于其不安全的特性，已经弃用
     */
    virtual void onRender() {};
    virtual bool onDestroy() = 0;

    // 从OpenCore 25.9 开始，以Draw替代onRender();
    virtual void Draw() {};

    virtual void onEnter() {};
    virtual void onExit() {};

    // 获取逻辑碰撞箱
    virtual SDL_Rect getLogicalBounds();

    // 获取实际碰撞箱
    virtual SDL_Rect getPhysicalBounds();

    // 更改贴图
    void changeTexture(unique_ptr<Texture> newTexture);

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
    void setParentContainer(IDrawableObject *parentContainer);
    void setTransparency(float alpha);
    void setMagnetFactor(int Margin);

    // 辅助方法
    SDL_Rect magnetRect(const SDL_Rect &srcRect) const;
    VisualState *getVisualState() const { return VState.get(); }

    // 配置器方法
    AnimationPipeline Animate();
    DrawableConfigurator Configure();

  protected:
    std::string id;
    uint8_t layer = 0;
    unique_ptr<AnimationManager> AnimeManager;
    unique_ptr<VisualState> VState;
    uint16_t absWidth, absHeight;
    unique_ptr<Texture> texture;
    int magnetFactor = 0;
    bool absolutePosite = true;
    IDrawableObject *parentContainer = nullptr;
};

#endif //_IDRAWABLE_H_