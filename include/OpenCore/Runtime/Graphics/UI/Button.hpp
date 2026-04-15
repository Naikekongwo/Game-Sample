/**
 * @file Button.hpp
 * @brief 按钮 UI 控件。
 * 
 * 提供可交互的按钮组件，支持三种状态（普通、悬停、按下），
 * 可绑定点击回调函数，并根据状态显示纹理的不同帧。
 */

#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "OpenCore/Runtime/Graphics/IDrawableObject/UIElement.hpp"

#include <functional>
#include <memory>

/**
 * @enum ButtonState
 * @brief 按钮的交互状态。
 */
enum ButtonState
{
    Normal,   ///< 普通状态（未交互）
    Hovered,  ///< 鼠标悬停状态
    Pressed   ///< 鼠标按下状态
};

/**
 * @class Button
 * @brief 可点击的按钮控件。
 * 
 * 继承自 UIElement，根据鼠标事件改变状态，并自动切换纹理帧（纹理需包含三帧，
 * 按 Normal、Hovered、Pressed 顺序排列）。支持绑定点击回调函数。
 */
class Button : public UIElement
{
public:
    /**
     * @brief 构造按钮对象。
     * @param id      唯一标识符。
     * @param layer   渲染图层。
     * @param texture 按钮纹理（应包含至少三帧，顺序为 Normal, Hovered, Pressed）。
     */
    Button(const std::string &id, uint8_t layer, unique_ptr<Texture> texture);
    
    void handlEvents(SDL_Event &event, float totalTime) override;
    void Draw() override;
    void onUpdate(float totalTime) override;

    /**
     * @brief 设置按钮点击时的回调函数。
     * @param func 无参无返回值的回调函数。
     */
    void setOnClick(std::function<void()> func) { onClick = std::move(func); }

protected:
    ButtonState State = ButtonState::Normal;   ///< 当前按钮状态
    std::function<void()> onClick;             ///< 点击回调函数
    short soundID = 1002;                      ///< 点击时播放的音效 ID
};

#endif //_BUTTON_H_