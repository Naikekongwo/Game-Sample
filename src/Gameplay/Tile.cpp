#include "Eclipsea/Gameplay/Sprite/Tile.h"
#include "OpenCore.hpp"
#include "Runtime/Graphics/IDrawableObject/Texture.hpp"
#include <memory>

Tile::Tile() {
  this->id = "Tile-Regular";
  this->layer = 0;
  this->VState = std::make_unique<VisualState>();
  this->AnimeManager = std::make_unique<AnimationManager>();

  texture.reset();
}

void Tile::onEnter() {
  // 初始化纹理库（直接以资源名同步加载，旧数字 ID 已移除）
  auto *package = OpenEngine::getInstance().getPackageManager();
  TerrainTiles = std::make_unique<Texture>(
      8, 8, package ? package->getTextureAsync("terrain_terrain") : nullptr);
}

void Tile::Draw() {
  auto info = OpenEngine::getInstance().getGameInfo();

  Rect dstRect = getLogicalBounds();
  Rect windowRect{0, 0, static_cast<float>(info->_graphicsInfo.resolutionWidth),
                  static_cast<float>(info->_graphicsInfo.resolutionHeight)};

  if (visible(dstRect, windowRect) && VState->getAlpha() > 0.0f) {
    // 可见才绘制
    switch (type) {
    case TileType::Terrain: {
      Rect srcRect = TerrainTiles->getSubRect(TileID);
      TerrainTiles->Draw(&srcRect, &dstRect, 0.0, nullptr);
      break;
    }
    default:
      break;
    }
  }
}

bool Tile::onDestroy() { return true; }