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
    // 所有UI空间的公共方法实现

    SDL_Rect getBounds() override;

    virtual int setBakedTexture(bool isBaked) { return 0; }
    
    // 贴图预烘焙
    virtual bool preRenderTexture(SDL_Texture* texture) { return false;}

protected:
    // 预烘焙贴图 : 默认关闭
    // 烘焙贴图的地址 : 默认空
    bool bakedTexture = false;
    std::unique_ptr<Texture> baked_Texture;
};

#endif //_UIELEMENT_H_