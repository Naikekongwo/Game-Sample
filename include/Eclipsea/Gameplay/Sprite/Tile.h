#pragma once

#include "Eclipsea/Gameplay/Map/Geometry/MapStruct.hpp"
#include "Runtime/Graphics/IDrawableObject/Sprite.hpp"
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

    // 调试描边：是否在 Tile 内部绘制 1px 白色描边（由 MapExplorer 同步）
    void setDebugOutline(bool enable) { m_debugOutline = enable; }

  private:
    short TileID = 0;
    //   默认的Tile类型是Terrain
    TileType type = TileType::Terrain;

    bool m_debugOutline = false; // 调试描边开关

    // 纹理库
    unique_ptr<Texture> TerrainTiles;
};