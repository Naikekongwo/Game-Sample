#pragma once

// Backpack.h
// 背包类

#include <vector>
#include <cstdint>
#include <memory>

using Items = std::pair<std::shared_ptr<Item>, uint8_t>;

class Backpack
{
    // 初始化方法
    Backpack(uint8_t size) : capacity_(size) {};

    // 查询物品
    uint16_t countItem(uint16_t itemId) const;

    // 添加物品
    int AddItem(Items item);


    // 减少物品
    bool RemoveItem(Items item);
    
    private: 

    uint8_t capacity_;
    std::vector<Items> storage_;
};