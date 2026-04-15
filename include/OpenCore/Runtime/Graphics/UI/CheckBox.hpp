/**
 * @file CheckBox.hpp
 * @brief 复选框 UI 控件。
 * 
 * 提供可勾选/取消勾选的复选框组件，内部使用 ImageBoard 显示勾选状态，
 * 并支持绑定外部 bool 变量以同步状态。
 */

#pragma once

#include "OpenCore/Runtime/Graphics/IDrawableObject/UIElement.hpp"
#include "OpenCore/Runtime/Graphics/UI/ImageBoard.hpp"
#include <memory>

using std::shared_ptr;
using std::string;

/**
 * @enum CheckBoxStatus
 * @brief 复选框的内部状态机。
 */
enum class CheckBoxStatus
{
    Creating,   ///< 创建中（尚未初始化）
    Ready,      ///< 就绪，可响应点击
    Checking    ///< 正在点击中（等待释放以确认勾选）
};

/**
 * @class CheckBox
 * @brief 复选框控件。
 * 
 * 继承自 UIElement，通过 ImageBoard 显示两张纹理帧（未勾选/勾选）。
 * 支持绑定外部 shared_ptr<bool>，点击时自动翻转其值。
 */
class CheckBox : public UIElement
{
public:
    /**
     * @brief 构造复选框对象。
     * @param id      唯一标识符。
     * @param layer   渲染图层。
     * @param texture 纹理（应包含两帧：帧0 = 未勾选，帧1 = 勾选）。
     */
    CheckBox(const string &id, short layer, unique_ptr<Texture> texture);
    void handlEvents(SDL_Event &event, float totalTime) override;
    // void onUpdate(float totalTime) override;   // 未使用，注释保留
    void Draw() override;
    bool onDestroy() override;
    void onEnter() override;
    void onExit() override;

    /**
     * @brief 绑定外部 bool 变量，勾选状态将同步到此变量。
     * @param value 外部 bool 的共享指针。
     */
    void bindVariable(shared_ptr<bool> value) { Value = value; }

    /**
     * @brief 刷新显示状态（根据 Value 更新 ImageBoard 的帧索引）。
     */
    void refreshStatus();

private:
    unique_ptr<ImageBoard> Img;              ///< 内部图像板，负责显示勾选框图形
    shared_ptr<bool> Value = std::make_shared<bool>(false); ///< 绑定的布尔值（默认 false）
    CheckBoxStatus status = CheckBoxStatus::Creating;       ///< 当前状态
};