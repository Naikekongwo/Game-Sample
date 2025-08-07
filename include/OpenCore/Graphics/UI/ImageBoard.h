// ImageBoard.h
// 图片类
#ifndef _IMAGEBOARD_H_
#define _IMAGEBOARD_H_

#include "../UIElement.h"
#include "OpenCore/Animation/AnimationPipeline.h"

class ImageBoard : public UIElement
{
    public:
    // 构造函数
    ImageBoard(short id, short layer, std::unique_ptr<Texture> texture);

    // 对于父类的实现
    void handlEvents(SDL_Event &event, float totalTime) override;
    void onRender() override;
    void onUpdate(float totalTime) override;
    void PushAnimation(short id, std::shared_ptr<IAnimation> Anime) override;

    SDL_Rect getBounds() override;

    AnimationPipeline Animate(); // 声明链式动画接口
};

#endif //_IMAGEBOARD_H_