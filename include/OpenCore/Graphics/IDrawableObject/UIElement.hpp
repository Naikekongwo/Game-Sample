#ifndef _UIELEMENT_H_
#define _UIELEMENT_H_

// UIElement.hpp
// UI控件的基类

#include "IDrawableObject.hpp"

class UIElement : public IDrawableObject
{
public:
    virtual OpenCore_Rect getBounds() override;
    virtual OpenCore_Rect getRenderedBounds() override;

    virtual void onRender() override;
    virtual int setBakedTexture(bool isBaked) { return 0; }
    virtual bool preRenderTexture(OpenCore_Tilemap* texture) { return false;}

    virtual bool onDestroy() override;

protected:
    // 预烘焙贴图 : 默认关闭
    // 烘焙贴图的地址 : 默认空
    bool directRender = false;
    std::unique_ptr<OpenCore_Tilemap, Tilemap_Deleter> TextureBuffer;
};

#endif //_UIELEMENT_H_