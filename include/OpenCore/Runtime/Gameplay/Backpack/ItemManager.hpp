#pragma once

// ItemManager.hpp
// 物品注册与创建管理器

#include "Item.hpp"
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

    /**
     * @brief 获取该物品类型的最大堆叠数量
     *
     * @param typeID
     * @return uint8_t
     */
    uint8_t getMaxStackSize(short typeID) const;
    /**
     * @brief 注册一个物品类型
     *
     * @param regInfo
     * @return true
     * @return false
     */
    bool registerItem(const ItemInfo &regInfo);
    /**
     * @brief 注册一个物品纹理贴图
     *
     * @param meta
     * @return true
     * @return false
     */
    bool registerItemTextureMeta(const ItemTextureMeta &meta);

    /**
     * @brief 不关心类型细节的委托构造方式
     *
     * @param typeID
     * @param owner
     * @return Item
     */
    std::optional<Item> createItem(short typeID);

    std::optional<ItemTextureMeta> getTextureMeta(short metaID);

  private:
    ItemManager() = default;

    unordered_map<short, ItemInfo> itemRegistry; // 类型 ID -> 注册信息
    unordered_map<short, ItemTextureMeta> itemTextureRegistry;
};