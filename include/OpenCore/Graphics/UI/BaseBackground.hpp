#pragma once

// BaseBackground.hpp
// 每一层级的背景的UI控件

#include "OpenCore/Graphics/IDrawableObject/UIElement.hpp"
#include <cstdint>

using std::string;

enum class BaseBackgroundStatus
{
    empty,
    ready
};

class BaseBackground : public UIElement
{
  public:
    // 构造函数
    BaseBackground(const string &id, uint8_t layer,
                   unique_ptr<Texture> texture);

    // 实现父类
    bool generateTexture(SDL_Texture *texture) override;

    void handlEvents(SDL_Event &event, float totalTime) override;
    void onUpdate(float totalTime) override;
    void onEnter() override;
    void onExit() override;
    void onRender() override;

    // 特殊函数
    void setNativeScale(uint8_t scale);

  private:
    uint8_t nativeScale = 60;
    BaseBackgroundStatus status = BaseBackgroundStatus::empty;

    SDL_Texture *TextureCache = nullptr;
};