#ifndef _BUTTON_H_
#define _BUTTON_H_

// Button.hpp
// 按钮类

#include "OpenCore/Graphics/IDrawableObject/UIElement.hpp"

#include <functional>
#include <memory>


// 按钮状态的枚举类
enum ButtonState { Normal, Hovered, Pressed };

class Button : public UIElement
{
    public:
    // 构造函数
    Button(const std::string& id, uint8_t layer, std::unique_ptr<Texture> texture);

    // 实现父类

    void handlEvents(SDL_Event &event, float totalTime) override;
    void onRender() override;
    void onUpdate(float totalTime) override;

    // 特殊函数

    void setOnClick(std::function<void()> func) { onClick = std::move(func); }

    protected:

    // 按钮状态 0：正常 1：悬停 2：按下
    ButtonState State = ButtonState::Normal;

    std::function<void()> onClick;

};

#endif //_BUTTON_H_