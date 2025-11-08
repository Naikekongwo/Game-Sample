#pragma once

// ItemConsumable.hpp
// 可消耗类型的物品

#include "Item.hpp"

#include <cstdint>
#include <string>


class ItemConsumable : public Item
{
    public:

    // 分别为增加堆叠数量和减少数量
    int16_t changeAmount(uint8_t src, int16_t offset);

    
};