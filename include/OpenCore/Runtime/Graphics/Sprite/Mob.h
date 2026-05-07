#pragma once

// Mob.h
// 生物的渲染类

#include "OpenCore//Runtime//Graphics//IDrawableObject//Sprite.hpp"
#include <memory>

using std::shared_ptr;

class Mob : public Sprite
{
  public:
    Mob(short MobTextureID);

    void Draw() override;

    bool onDestroy() override { return true; };

  private:
    shared_ptr<Texture> shadow;
};