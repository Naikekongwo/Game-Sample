#pragma once

// Item.hpp
// 物品的基类

#include <cstdint>

#define ILEGAL_ITEM 0

class Item
{
  public:
    Item() = default;

    Item(short ID, uint8_t maxAmount) : ID(ID), maxAmount(maxAmount) {}

    short getID() const noexcept { return ID; }
    uint8_t getAmount() const noexcept { return Amount; }

    bool setAmount(uint8_t amount) const noexcept
    {
        if (amount >= 0 && amount <= maxAmount)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

  protected:
    short ID = 0;
    uint8_t Amount = 1;

    uint8_t maxAmount = 0;
};