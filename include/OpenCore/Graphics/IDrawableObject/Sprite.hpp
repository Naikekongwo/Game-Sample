#ifndef _SPRITE_H_
#define _SPRITE_H_

// Sprite.hpp
// 精灵类的基类

#include "IDrawableObject.hpp"

class Sprite : public IDrawableObject
{
  public:
    virtual SDL_Rect getLogicalBounds() override;

    virtual SDL_Rect getPhysicalBounds() override;
};

#endif //_SPRITE_H_