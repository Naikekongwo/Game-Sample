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
#include "OpenCore/Runtime/Graphics/UI/ImageBoard.hpp"

#include <memory>

using std::shared_ptr;
using std::string_view;
using std::weak_ptr;

class HealthBar : public Sprite
{
  public:
    HealthBar(string_view id, short layer, unique_ptr<Texture> texture);
    ~HealthBar() override = default;

    void onEnter() override;
    void Draw() override;

    bool onDestroy() override { return true; };

  private:
    // 生命值比例
    weak_ptr<float> m_healthPercent;
};