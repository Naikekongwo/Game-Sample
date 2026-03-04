#pragma once

#include "OpenCore/Runtime/Graphics/IDrawableObject/Sprite.hpp"
#include <memory>

enum class TileType
{
    Terrain,
};

class Tile : public Sprite
{
  public:
    Tile();

    void onEnter() override;
    void Draw() override;

    bool onDestroy() override;

    // 设置块的类型
    void setTileType(TileType Type) { this->type = Type; }
    void setTileID(short tileID) { this->TileID = tileID; }

  private:
    short TileID = 0;
    //   默认的Tile类型是Terrain
    TileType type = TileType::Terrain;

    // 纹理库
    unique_ptr<Texture> TerrainTiles;
};