#pragma once

// ItemContainer.hpp
// 定义了物品栏的最小单位的控件

#include "UIElement.hpp"

class Item;

class ItemContainer : public UIElement
{

    // 继承自UIElement
    public:
    
    // 构造方法
    ItemContainer(std::string id, short layer, Texture* texture);

    // 父类实现

    // void handlEvents(SDL_Event &event, float totalTime) override;

    void onRender() override;

    // void onUpdate(float totalTime) override;

    protected:
    // 一定会在内部加入储存 item 类的 ID,但是目前不需要   
    
};