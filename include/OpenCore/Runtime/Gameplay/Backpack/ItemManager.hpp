#pragma once

// ItemManager.hpp
// 负责注册物品的管理器

#include <memory>
#include <unordered_map>

#include "Item.hpp"

using std::unordered_map;

static Item emptyItem;

struct ItemInfo
{
    short ItemTypeID = 0;
    uint8_t ItemMaxSize = 0;
};

class ItemManager final
{
  public:
    // 单例的操作模式
    static ItemManager &getInstance()
    {
        static ItemManager instance;
        return instance;
    }

    // 生产特定的Item
    Item createItem(short ItemTypeID);

  private:
    unordered_map<short, ItemInfo> ItemInfos;
};