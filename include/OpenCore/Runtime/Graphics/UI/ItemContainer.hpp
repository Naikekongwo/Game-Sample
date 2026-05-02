/**
 * @file ItemContainer.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2026-04-27
 *
 * @copyright Copyright (c) 2026
 *
 */

#pragma once

#include "OpenCore/Runtime/Gameplay/Backpack/Backpack.hpp"
#include "OpenCore/Runtime/Graphics/IDrawableObject/UIElement.hpp"

#include <memory>
#include <string>

using std::pair;
using std::shared_ptr;
using std::string;
using std::string_view;
using std::weak_ptr;

class Backpack;
class ItemSprite;

class ItemContainer : public UIElement
{
  public:
    ItemContainer(string_view id, uint8_t layer, unique_ptr<Texture> texture,
                  short col, short row);

    void handlEvents(SDL_Event &event, float totalTime) override {};
    void onUpdate(float totalTime) override {};
    void onEnter() override;
    void onExit() override {};
    void Draw() override;

    void setIndexRange(pair<uint8_t, uint8_t> indexRange);
    void setBackpack(shared_ptr<Backpack> backpack);

  private:
    weak_ptr<Backpack> m_backpack;
    pair<uint8_t, uint8_t> m_indexRange;

    short m_columns = 1;

    unique_ptr<ItemSprite> m_item;
};