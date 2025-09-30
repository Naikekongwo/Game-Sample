#ifndef _BUTTON_H_
#define _BUTTON_H_

// Button.hpp
// 按钮类

#include "UIElement.hpp"


// 按钮状态的枚举类
enum ButtonState { Normal, Hovered, Pressed };

class Button : public UIElement
{
    public:
    // 构造函数
    Button(std::string id, short layer, Texture* texture);

    // 实现父类

    void handlEvents(SDL_Event &event, float totalTime) override;
    void onRender() override;
    void onUpdate(float totalTime) override;

    // 特殊函数

    protected:

    // 按钮状态 0：正常 1：悬停 2：按下
    ButtonState State = ButtonState::Normal;

};

#endif //_BUTTON_H_