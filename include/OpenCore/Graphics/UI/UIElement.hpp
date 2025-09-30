#ifndef _UIELEMENT_H_
#define _UIELEMENT_H_

// UIElement.hpp
// UI控件的基类

#include "../IDrawableObject.hpp"

class UIElement : public IDrawableObject
{
    public:


    // 所有UI空间的公共方法实现

    SDL_Rect getBounds() override;
    
};

#endif //_UIELEMENT_H_