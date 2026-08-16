/**
 * @file Item.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2026-04-27
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "Runtime/Graphics/IDrawableObject/Sprite.hpp"

#include <string>

class ItemSprite : public Sprite {
public:
  ItemSprite();
  explicit ItemSprite(std::string_view textureName); // 接受纹理资源名
  ~ItemSprite() override = default;

  void onEnter() override;
  void Draw() override;

  bool onDestroy() override { return true; };

  bool setSubTexture(short index);

private:
  // 正常来说，其不应该持有这个
  std::string itemTexName = "items_sets00";
};