#ifndef _IMAGEBOARD_H_
#define _IMAGEBOARD_H_

// ImageBoard.hpp
// 图片类

#include <string>

#include "UIElement.hpp"
#include "OpenCore/Animation/AnimationPipeline.hpp"

class ImageBoard : public UIElement
{
    public:
    // 构造函数
    ImageBoard(std::string id, short layer, Texture* texture);

    // 对于父类的实现
    void onRender() override;


    // 事件处理和更新被废除,因为图片框无需考虑这两者
    // void handlEvents(SDL_Event &event, float totalTime) override;
    // void onUpdate(float totalTime) override;

};

#endif //_IMAGEBOARD_H_