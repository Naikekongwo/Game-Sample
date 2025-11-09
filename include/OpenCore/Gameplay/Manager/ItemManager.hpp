#pragma once

// ItemManager.hpp
// 物品管理器
// （注册）

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "OpenCore/Gameplay/Items/Item.hpp"

using Items = std::pair<std::shared_ptr<Item>, uint8_t>;

// 类之定义
class ItemManager
{
    public:

    // 单例
    static ItemManager& getInstance()
    {
        static ItemManager instance;
        return instance;
    }

    // 注册物品
    bool registerItem(std::shared_ptr<Item> item);
    // 修改物品
    std::shared_ptr<Item> modifyItem(uint16_t itemId);
    // 删除物品
    bool deleteItem(uint16_t itemId);
    // 查找物品是否存在
    bool countItemById(uint16_t id);


    private:
    
    std::unordered_map<uint16_t, std::shared_ptr<Item>> items_;

};