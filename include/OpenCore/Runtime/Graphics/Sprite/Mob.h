#pragma once

// Mob.h
// 生物的渲染类

#include "OpenCore//Runtime//Graphics//IDrawableObject//Sprite.hpp"
#include <memory>

using std::shared_ptr;

class Mob : public Sprite
{
  public:
    Mob(short MobTextureID, uint8_t gridCols = 4, uint8_t gridRows = 4);

    void Draw() override;

    bool onDestroy() override { return true; };

    void setTileSize(uint8_t w, uint8_t h) { tileWidth = w; tileHeight = h; }
    uint8_t getTileWidth() const { return tileWidth; }
    uint8_t getTileHeight() const { return tileHeight; }

  private:
    shared_ptr<Texture> shadow;
    uint8_t tileWidth = 1;
    uint8_t tileHeight = 1;
};