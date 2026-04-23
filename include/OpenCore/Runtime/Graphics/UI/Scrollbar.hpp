/**
 * @file Scrollbar.hpp
 * @brief 滑动条控件（滑块）。
 * 
 * 提供一个水平滑动条，包含背景和滑块两部分，支持鼠标拖拽改变绑定的浮点数值（0~1）。
 */

#pragma once

#include "OpenCore/Runtime/Graphics/IDrawableObject/UIElement.hpp"
#include <memory>

using std::make_shared;
using std::shared_ptr;
using std::string;

/**
 * @enum ScrollStatus
 * @brief 滑动条的交互状态。
 */
enum class ScrollStatus
{
    Creating,   ///< 创建中，尚未初始化
    Ready,      ///< 就绪，可响应点击
    Following,  ///< 正在拖拽滑块
};

class ImageBoard;      ///< 前向声明（滑块控件）
class BaseBackground;  ///< 前向声明（背景条控件）

/**
 * @class Scrollbar
 * @brief 水平滑动条控件。
 * 
 * 由背景条（BaseBackground）和滑块（ImageBoard）组成，支持鼠标拖拽。
 * 拖拽时更新绑定的 float 值（范围 0.0 ~ 1.0），滑块位置随之移动。
 */
class Scrollbar : public UIElement
{
public:
    /**
     * @brief 构造滑动条对象。
     * @param id         唯一标识符。
     * @param layer      渲染图层。
     * @param backTexID  背景纹理的资源 ID。
     * @param buttTexID  滑块纹理的资源 ID。
     */
    Scrollbar(const string &id, short layer, short backTexID, short buttTexID);
    void handlEvents(SDL_Event &event, float totalTime) override;
    void onUpdate(float totalTime) override;
    void Draw() override;
    bool onDestroy() override;
    void onEnter() override;
    void onExit() override;

    /**
     * @brief 获取滑块子控件指针。
     * @return ImageBoard* 滑块控件指针。
     */
    ImageBoard *getSlideBar() const { return slideBar.get(); }

    /**
     * @brief 获取背景条子控件指针。
     * @return BaseBackground* 背景条控件指针。
     */
    BaseBackground *getBase() const { return baseBack.get(); }

    /**
     * @brief 绑定外部浮点数值，滑块位置将同步到此变量。
     * @param value 外部 float 的共享指针（范围 0.0 ~ 1.0）。
     */
    void bindVariable(shared_ptr<float> value) { this->value = value; }

    /**
     * @brief 根据当前绑定的数值更新滑块位置。
     */
    void UpdateBar();

private:
    short backgroundTexture = 2009;              ///< 背景纹理资源 ID
    short buttonTexture = 2026;                  ///< 滑块纹理资源 ID
    unique_ptr<ImageBoard> slideBar;             ///< 滑块子控件
    unique_ptr<BaseBackground> baseBack;         ///< 背景条子控件
    shared_ptr<float> value;                     ///< 绑定的数值（0~1）
    ScrollStatus status = ScrollStatus::Creating;///< 当前交互状态
};