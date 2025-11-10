#pragma once

// ItemManager.hpp
// 物品管理器
// （注册）

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "OpenCore/Gameplay/Items/Item.hpp"
#include "OpenCore/Gameplay/Items/ItemContainer.hpp"

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

    // 使用id取得对应的物品
    std::shared_ptr<Item> getItem(uint16_t itemId);

    // 使用name取得对应的物品
    std::shared_ptr<Item> getItem(const std::string& name);


    private:

    ItemManager();
    
    std::unordered_map<uint16_t, std::shared_ptr<Item>> items_;

};