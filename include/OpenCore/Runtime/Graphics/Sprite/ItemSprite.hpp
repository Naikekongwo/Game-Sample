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

#include "OpenCore/Runtime/Graphics/IDrawableObject/Sprite.hpp"

class ItemSprite : public Sprite
{
  public:
    ItemSprite();

    void onEnter() override;
    void Draw() override;

    bool onDestroy() override { return true; };

  private:
    // 正常来说，其不应该持有这个
    short itemTexID = 2038;
};