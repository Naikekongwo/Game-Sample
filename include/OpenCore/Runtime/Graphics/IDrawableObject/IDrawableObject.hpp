/**
 * @file IDrawable.hpp
 * @brief 可绘制对象的基类接口。
 *
 * 定义了所有可绘制对象的基本行为：事件处理、更新、渲染、生命周期回调、
 * 碰撞箱、动画管理、配置接口等。
 */
#ifndef _IDRAWABLE_H_
#define _IDRAWABLE_H_

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
/**
 * @class IDrawableObject
 * @brief 可绘制对象的抽象基类。
 *
 * 所有需要在场景中显示、具有位置、缩放、透明度、动画等属性的对象都应继承此类。
 * 提供统一的生命周期管理、碰撞箱获取、属性设置以及动画和配置器入口。
 */
class IDrawableObject
{
  public:
    IDrawableObject();

    virtual ~IDrawableObject() = default;

    /**
     * @brief 处理 SDL 事件。
     * @param event SDL 事件引用。
     * @param totalTime 从应用启动至今的总时间（毫秒）。
     */
    virtual void handlEvents(SDL_Event &event, float totalTime);
    /**
     * @brief 每帧更新逻辑（例如动画、状态机）。
     * @param totalTime 从应用启动至今的总时间。
     */
    virtual void onUpdate(float totalTime);
    /**
     * @brief 非原生渲染方法
     * @deprecated 由于其不安全的特性，已经弃用
     */
    virtual void onRender() {};
    /**
     * @brief 对象销毁前的回调，必须实现。
     * @return true 表示允许销毁，false 表示取消销毁。
     */
    virtual bool onDestroy() = 0;

    /**
     * @brief 渲染方法（推荐使用）。从 OpenCore 25.9 开始替代 onRender()。
     */
    virtual void Draw() {};
    /**
     * @brief 当对象被添加到场景时调用。
     */
    virtual void onEnter() {};
    /**
     * @brief 当对象从场景中移除时调用。
     */
    virtual void onExit() {};

    /**
     * @brief 获取逻辑碰撞箱。
     * @return SDL_Rect 碰撞箱矩形。
     */
    virtual SDL_Rect getLogicalBounds();

    /**
     * @brief 获取实际物理碰撞箱。
     * @return SDL_Rect 碰撞箱矩形。
     */
    virtual SDL_Rect getPhysicalBounds();

    /**
     * @brief 更改当前显示的贴图（纹理）。
     * @param newTexture 纹理智能指针。
     */
    void changeTexture(unique_ptr<Texture> newTexture);

    /**
     * @brief 设置动画是否为顺序播放（而非循环或随机）。
     * @param sequential true 表示顺序播放，false 表示并行模式。
     */
    void setSequential(bool sequential);

    /**
     * @brief 获取对象的唯一标识符。
     * @return 字符串 ID。
     */
    std::string getID() const { return id; }
    /**
     * @brief 获取对象的渲染图层（值越小越靠后）。
     * @return 图层编号。
     */
    short getLayer() const { return layer; }
    /**
     * @brief 检查动画是否已完成播放。
     * @return true 表示动画完成，false 表示未完成。
     */
    bool isAnimeFinished() const;

    /**
     * @brief 设置缩放比例。
     * @param w 宽度缩放因子（1.0 表示原始宽度）。
     * @param h 高度缩放因子（1.0 表示原始高度）。
     */
    void setScale(float w, float h);
    /**
     * @brief 设置锚点位置（决定位置坐标的参考点，九宫格形状，用于旋转缩放）。
     * @param anchor 锚点枚举值（例如左上、中心等）。
     */
    void setAnchor(AnchorPoint anchor);
    /**
     * @brief 设置对象的位置（使用百分比坐标）。
     * @param xPercent 相对于父容器宽度的百分比（0.0 ~ 1.0）。
     * @param yPercent 相对于父容器高度的百分比（0.0 ~ 1.0）。
     */
    void setPosition(float xPercent, float yPercent);
    /**
     * @brief 设置父容器对象。
     * @param parentContainer 父对象的指针。
     */
    void setParentContainer(IDrawableObject *parentContainer);
    /**
     * @brief 设置透明度（Alpha 混合值）。
     * @param alpha 透明度范围（0.0 完全透明 ~ 1.0 完全不透明）。
     */
    void setTransparency(float alpha);
    /**
     * @brief 设置磁吸边距（用于自动布局或边缘吸附）。
     * @param Margin 边距像素值。
     */
    void setMagnetFactor(int Margin);

    /**
     * @brief 根据磁吸因子和源矩形计算目标矩形位置。
     * @param srcRect 源矩形（通常为对象当前矩形）。
     * @return SDL_Rect 调整后的矩形。
     */
    SDL_Rect magnetRect(const SDL_Rect &srcRect) const;
    /**
     * @brief 获取对象的视觉状态（包含位置、缩放、透明度等运行时信息）。
     * @return VisualState 的智能指针。
     */
    VisualState *getVisualState() const { return VState.get(); }

    /**
     * @brief 获取动画管道对象，用于链式定义动画。
     * @return AnimationPipeline 对象（临时值）。
     */
    AnimationPipeline Animate();
    /**
     * @brief 获取配置器对象，用于链式设置对象属性。
     * @return DrawableConfigurator 对象（临时值）。
     */
    DrawableConfigurator Configure();

  protected:
    std::string id;                            ///< 对象唯一标识符
    uint8_t layer = 0;                         ///< 渲染图层（0 最低）
    unique_ptr<AnimationManager> AnimeManager; ///< 动画管理器
    unique_ptr<VisualState> VState;            ///< 视觉状态（位置、缩放等）
    uint16_t absWidth, absHeight;              ///< 原始宽高（像素）
    unique_ptr<Texture> texture;               ///< 当前纹理
    int magnetFactor = 0;                      ///< 磁吸因子（像素）
    bool absolutePosite = true; ///< 位置是否基于绝对坐标（否则为相对父容器）
    IDrawableObject *parentContainer = nullptr; ///< 父容器指针
};

#endif //_IDRAWABLE_H_