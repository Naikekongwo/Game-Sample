/**
 * @file BaseBackground.hpp
 * @brief 背景层 UI 控件，支持九宫格拉伸缩放。
 *
 * 提供基于九宫格（3x3 网格）的纹理绘制，可指定原生缩放像素大小，
 * 用于实现可拉伸的背景、面板等元素。
 */

#pragma once

#include "OpenCore/Runtime/Graphics/IDrawableObject/UIElement.hpp"
#include <cstdint>

using std::string;

class ItemContainer;

/**
 * @enum BaseBackgroundStatus
 * @brief 背景资源准备状态。
 */
enum class BaseBackgroundStatus
{
    empty, ///< 未生成缓存纹理
    ready  ///< 缓存纹理已准备就绪
};

/**
 * @class BaseBackground
 * @brief 可九宫格拉伸的背景 UI 控件。
 *
 * 继承自 UIElement，通过 3x3 网格划分原始纹理，根据目标矩形尺寸和
 * nativeScale 自动拉伸四个边和中间区域，实现无失真缩放。
 */
class BaseBackground : public UIElement
{
  public:
    /**
     * @brief 构造背景控件。
     * @param id      唯一标识符。
     * @param layer   渲染图层。
     * @param texture 原始纹理（将被分割为 3x3 网格）。
     */
    BaseBackground(const string &id, uint8_t layer,
                   unique_ptr<Texture> texture);
    bool generateTexture(SDL_Texture *texture) override;
    void handlEvents(SDL_Event &event, float totalTime) override;
    void onUpdate(float totalTime) override;
    void onEnter() override;
    void onExit() override;
    void Draw() override;

    /**
     * @brief 设置原生缩放像素大小（九宫格边角的固定像素宽度）。
     * @param scale 边角大小（像素）。
     */
    void setNativeScale(uint8_t scale);

  private:
    uint8_t nativeScale = 60; ///< 九宫格边角缩放基数（像素）
    BaseBackgroundStatus status = BaseBackgroundStatus::empty; ///< 当前状态
    SDL_Texture *TextureCache = nullptr; ///< 离屏缓存纹理指针
};
