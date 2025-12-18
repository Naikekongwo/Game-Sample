#ifndef _SPRITE_H_
#define _SPRITE_H_

// Sprite.hpp
// 精灵类的基类

#include "IDrawableObject.hpp"

class Sprite : public IDrawableObject
{
    public:

    virtual SDL_Rect getBounds() override;

    virtual SDL_Rect getRenderedBounds() override;

};

#endif //_SPRITE_H_