/**
 * @file FrameCounter.hpp
 * @brief 帧率计数器 UI 控件。
 * 
 * 用于实时计算并显示当前渲染帧率（FPS），通常作为调试或性能监控工具。
 */

#pragma once

#include "OpenCore/Runtime/Graphics/IDrawableObject/UIElement.hpp"

/**
 * @class FrameCounter
 * @brief 帧率显示器。
 * 
 * 继承自 UIElement，在每一帧更新时计算时间差，得出当前 FPS，
 * 并通过内部字体 ID（fontID）将 FPS 数值渲染为文本。
 */
class FrameCounter : public UIElement
{
public:
    /**
     * @brief 构造帧率计数器对象。
     * @param id      唯一标识符。
     * @param layer   渲染图层。
     * @param texture 用于显示文本的纹理。
     */
    FrameCounter(const std::string &id, uint8_t layer, Texture *texture);
    void handlEvents(SDL_Event &event, float totalTime) override;
    void Draw() override;
    void onUpdate(float totalTime) override;

private:
    float lastTime = 0.0f;   ///< 上一帧的时间戳
    float FPS = 0.0f;        ///< 当前计算的帧率
    short fontID = 9001;     ///< 用于渲染 FPS 文本的字体资源 ID
};