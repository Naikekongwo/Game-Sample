#ifndef _UIELEMENT_H_
#define _UIELEMENT_H_

// UIElement.hpp
// UI控件的基类

#include "../IDrawableObject.hpp"

template <typename T>
std::unique_ptr<T> UI(std::string id, short layer, short texID, short frameX, short frameY);

// 引入工厂方法
#include "UIFactory.inl"

class UIElement : public IDrawableObject
{
public:
    SDL_Rect getBounds() override;

    virtual void onRender() override;
    virtual int setBakedTexture(bool isBaked) { return 0; }
    virtual bool preRenderTexture(SDL_Texture* texture) { return false;}

protected:
    // 预烘焙贴图 : 默认关闭
    // 烘焙贴图的地址 : 默认空
    bool directRender = false;
    std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> TextureBuffer { nullptr, SDL_DestroyTexture};
};

#endif //_UIELEMENT_H_