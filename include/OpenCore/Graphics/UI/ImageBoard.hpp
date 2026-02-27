#ifndef _IMAGEBOARD_H_
#define _IMAGEBOARD_H_

// ImageBoard.hpp
// 图片类

#include <string>

#include "OpenCore/Graphics/IDrawableObject/UIElement.hpp"

class ImageBoard : public UIElement
{
  public:
    // 构造函数
    ImageBoard(const std::string &id, uint8_t layer,
               std::unique_ptr<Texture> texture);

    // 对于父类的实现
    void onRender() override;
};

#endif //_IMAGEBOARD_H_