#pragma once

// FrameCounter.hpp
// 帧率计数器的控件

#include "OpenCore/Graphics/IDrawableObject/UIElement.hpp"

class FrameCounter : public UIElement
{
    public:
    FrameCounter(std::string id, short layer, Texture *texture);

    // 继承

    void handlEvents(SDL_Event &event, float totalTime) override;
    void onRender() override;
    void onUpdate(float totalTime) override;

    private:
    float lastTime = 0.0f;
    float FPS = 0.0f;
    short fontID = 9001;
};