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

using std::shared_ptr;
using std::string;
using std::string_view;
using std::weak_ptr;

class Backpack;
class ItemSprite;

class ItemContainer : public UIElement
{
  public:
    ItemContainer(const string &id, uint8_t layer, unique_ptr<Texture> texture,
                  short col, short row);

    void handlEvents(SDL_Event &event, float totalTime) override {};
    void onUpdate(float totalTime) override {};
    void onEnter() override;
    void onExit() override {};
    void Draw() override;

    void setBackpack(shared_ptr<Backpack> backpack);
    void setSize(short row, short col);

  private:
    float maxHeight = 1.0f;
    short rows = 1;
    short columns = 1;
    weak_ptr<Backpack> m_backpack;

    unique_ptr<ItemSprite> m_item;
};