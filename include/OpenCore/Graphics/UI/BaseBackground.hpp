#pragma once

// BaseBackground.hpp
// 每一层级的背景的UI控件

#include "OpenCore/Graphics/IDrawableObject/UIElement.hpp"

#include <cstdint>

class BaseBackground : public UIElement
{
    public:
    // 构造函数
    BaseBackground(const std::string& id, uint8_t layer, std::unique_ptr<Texture> texture);

    // 实现父类

    void handlEvents(SDL_Event &event, float totalTime) override;
    int setBakedTexture(bool isBaked) override;
    bool preRenderTexture(SDL_Texture* texture) override;

    // 特殊函数

    void setNativeScale(uint8_t scale);


    private:

    uint8_t nativeScale;
};