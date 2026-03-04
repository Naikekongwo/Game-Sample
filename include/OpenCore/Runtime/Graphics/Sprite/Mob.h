#pragma once

// Mob.h
// 生物的渲染类

#include "OpenCore//Runtime//Graphics//IDrawableObject//Sprite.hpp"

class Mob : public Sprite
{
  public:
    Mob()
    {
        this->id = "Mob";
        this->layer = 0;
    }

  private:
};