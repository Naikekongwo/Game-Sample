#ifndef _UIELEMENT_H_
#define _UIELEMENT_H_

// UIElement.hpp
// UI控件的基类

#include "IDrawableObject.hpp"

class UIElement : public IDrawableObject
{
  public:
    UIElement(const string &id, short layer, unique_ptr<Texture> texture);

    virtual SDL_Rect getLogicalBounds() override;
    virtual SDL_Rect getPhysicalBounds() override;

    virtual bool generateTexture(SDL_Texture *texture) { return false; }

    virtual bool onDestroy() override;
};

#endif //_UIELEMENT_H_