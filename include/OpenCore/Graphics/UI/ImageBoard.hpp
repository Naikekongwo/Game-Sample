// ImageBoard.hpp
// 图片类
#ifndef _IMAGEBOARD_H_
#define _IMAGEBOARD_H_

#include <string>

#include "../UIElement.hpp"
#include "OpenCore/Animation/AnimationPipeline.hpp"

class ImageBoard : public UIElement
{
    public:
    // 构造函数
    ImageBoard(std::string id, short layer, Texture* texture);

    // 对于父类的实现
    void handlEvents(SDL_Event &event, float totalTime) override;
    void onRender() override;
    void onUpdate(float totalTime) override;
    void changeTexture(Texture* newTexture) override;

    SDL_Rect getBounds() override;

};

#endif //_IMAGEBOARD_H_