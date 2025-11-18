#ifndef _UIELEMENT_H_
#define _UIELEMENT_H_

// UIElement.hpp
// UI控件的基类

#include "IDrawableObject.hpp"

class UIElement : public IDrawableObject
{
public:
    virtual SDL_Rect getBounds() override;
    virtual SDL_Rect getRenderedBounds() override;

    virtual void onRender() override;
    virtual int setBakedTexture(bool isBaked) { return 0; }
    virtual bool preRenderTexture(SDL_Texture* texture) { return false;}

    virtual bool onDestroy() override;

protected:
    // 预烘焙贴图 : 默认关闭
    // 烘焙贴图的地址 : 默认空
    bool directRender = false;
    std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> TextureBuffer { nullptr, SDL_DestroyTexture};
};

#endif //_UIELEMENT_H_