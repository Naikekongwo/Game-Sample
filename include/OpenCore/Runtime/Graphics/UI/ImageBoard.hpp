/**
 * @file ImageBoard.hpp
 * @brief 基础图片显示控件。
 *
 * 提供简单的图像显示功能，继承自 UIElement，支持纹理帧索引、透明度、
 * 磁吸边距等基础视觉特性。
 */

#ifndef _IMAGEBOARD_H_
#define _IMAGEBOARD_H_

#include "OpenCore/Runtime/Graphics/IDrawableObject/UIElement.hpp"
#include <string>

/**
 * @class ImageBoard
 * @brief 静态图片或单帧动画的显示控件。
 *
 * 用于显示单个纹理（或纹理中的某一帧），不包含交互逻辑。
 * 支持通过 VisualState 控制透明度、旋转角度、帧索引等。
 */
class ImageBoard : public UIElement
{
  public:
    /**
     * @brief 构造 ImageBoard 对象。
     * @param id      唯一标识符。
     * @param layer   渲染图层（值越小越靠后）。
     * @param texture 关联的纹理智能指针（不能为空）。
     */
    ImageBoard(const std::string &id, uint8_t layer,
               unique_ptr<Texture> texture);

    void Draw() override;
    void onUpdate(float totalTime) override;
};

#endif //_IMAGEBOARD_H_