#include "OpenCore/Runtime/Gameplay/Backpack/ItemManager.hpp"
#include "OpenCore/OpenCore.hpp"                           // LOG 宏等
#include "OpenCore/Runtime/Gameplay/Backpack/Backpack.hpp" // 需要 Backpack 的完整定义以调用 pushItem
#include "OpenCore/Runtime/Gameplay/Entity/Entity.hpp" // Entity 前向声明或完整定义
#include <optional>

uint8_t ItemManager::getMaxStackSize(short typeID) const
{
    auto it = itemRegistry.find(typeID);
    if (it != itemRegistry.end())
        return it->second.maxStackedAmount;
    return 64; // 默认堆叠上限
}
bool ItemManager::registerItem(const ItemInfo &regInfo)
{
    if (itemRegistry.contains(regInfo.typeID))
    {
        LOG("物品注册失败：类型 ID {} 已存在", regInfo.typeID);
        return false;
    }

    itemRegistry[regInfo.typeID] = regInfo;
    LOG("物品注册成功，名称 {} 类型 ID {}", regInfo.id, regInfo.typeID);
    return true;
}

optional<Item> ItemManager::createItem(short typeID)
{
    auto it = itemRegistry.find(typeID);
    if (it == itemRegistry.end())
    {
        LOG("创建物品失败：类型 ID {} 未注册", typeID);
        return std::nullopt;
    }

    // 已经注册的物品
    Item result(itemRegistry.at(typeID));
    return result;
}

bool ItemManager::registerItemTextureMeta(const ItemTextureMeta &meta)
{
    if (itemTextureRegistry.contains(meta.textureID))
    {
        ItemTextureMeta old = itemTextureRegistry.at(meta.textureID);
        if (old.texture_cols == meta.texture_cols &&
            old.texture_rows == meta.texture_rows)
        {
            // 完全相同
            return true;
        }
        else
        {
            return false;
        }
    }

    itemTextureRegistry[meta.textureID] = meta;
    LOG("注册了物品贴图元信息, 纹理ID: {} 纹理横向网格 {} 纵向网格 {}",
        meta.textureID, meta.texture_cols, meta.texture_rows);
    return true;
}

optional<ItemTextureMeta> ItemManager::getTextureMeta(short texID)
{
    if (!itemTextureRegistry.contains(texID))
    {
        return std::nullopt;
    }

    return itemTextureRegistry.at(texID);
}