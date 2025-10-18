#pragma once

// StageBackground.hpp
// 每一层级的背景的UI控件

#include "UIElement.hpp"

class StageBackground : public UIElement
{
    public:
    // 构造函数
    StageBackground(std::string id, short layer, Texture* texture);

    // 实现父类

    void handlEvents(SDL_Event &event, float totalTime) override;
    void onUpdate(float totalTime) override;
    int setBakedTexture(bool isBaked) override;
    bool preRenderTexture(SDL_Texture* texture) override;

    // 特殊函数

    void setNativeScale(int scale);


    private:

    int nativeScale;
};