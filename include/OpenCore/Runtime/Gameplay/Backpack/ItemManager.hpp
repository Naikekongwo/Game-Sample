#pragma once

// ItemManager.hpp
// 物品注册与创建管理器

#include "OpenCore/Runtime/Gameplay/Backpack/Item.hpp"
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

class ItemManager final
{
  public:
    static ItemManager &getInstance()
    {
        static ItemManager instance;
        return instance;
    }

    uint8_t getMaxStackSize(short typeID) const;
    bool registerItem(const ItemInfo &regInfo);

    /**
     * @brief 不关心类型细节的委托构造方式
     *
     * @param typeID
     * @param owner
     * @return Item
     */
    std::optional<Item> createItem(short typeID);

  private:
    ItemManager() = default;

    unordered_map<short, ItemInfo> itemRegistry; // 类型 ID -> 注册信息
};