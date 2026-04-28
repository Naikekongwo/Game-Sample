#include "OpenCore/Runtime/Gameplay/Backpack/ItemManager.hpp"
#include "OpenCore/OpenCore.hpp"       // LOG 宏等
#include "OpenCore/Runtime/Gameplay/Entity/Entity.hpp"                  // Entity 前向声明或完整定义
#include "OpenCore/Runtime/Gameplay/Backpack/Backpack.hpp"      // 需要 Backpack 的完整定义以调用 pushItem

uint8_t ItemManager::getMaxStackSize(short typeID) const
{
    auto it = itemRegistry.find(typeID);
    if (it != itemRegistry.end())
        return it->second.maxAmount;
    return 64; // 默认堆叠上限
}
bool ItemManager::registerItem(const ItemRegInfo &regInfo)
{
    if (itemRegistry.contains(regInfo.typeID))
    {
        LOG("物品注册失败：类型 ID {} 已存在", regInfo.typeID);
        return false;
    }

    itemRegistry[regInfo.typeID] = regInfo;
    LOG("物品注册成功，类型 ID：{}", regInfo.typeID);
    return true;
}

ItemInfo ItemManager::buildItemInfo(ItemRegInfo &regInfo,
                                    Entity* owner,
                                    unsigned int statueID)
{
    // 自动生成唯一实例 ID： "item_<typeID>_<allocatedCount>"
    string instanceID = "item_" + std::to_string(regInfo.typeID) + "_" 
                        + std::to_string(regInfo.allocatedCount);
    ++regInfo.allocatedCount;   // 递增计数器

    ItemInfo info;
    info.id          = instanceID;
    info.typeID      = regInfo.typeID;
    info.textureID   = regInfo.textureID;
    info.widthFactor = regInfo.widthFactor;
    info.statueID    = statueID;
    info.owner       = owner;
    return info;
}

Item ItemManager::createItem(short typeID, Entity* owner, unsigned int statueID)
{
    auto it = itemRegistry.find(typeID);
    if (it == itemRegistry.end())
    {
        LOG("创建物品失败：类型 ID {} 未注册", typeID);
        return Item();
    }

    ItemInfo info = buildItemInfo(it->second, owner, statueID);
    Item item;
    item.Configure(info);   // 配置物品，包括设定 owner

    // 如果指定了归属实体，尝试自动放入其背包
    if (owner)
    {
        auto backpack = owner->getBackpack();   // 返回 shared_ptr<Backpack>
        if (backpack)
        {
            if (backpack->pushItem(item))
            {
                LOG("物品 {} 已自动放入实体背包", item.getID());
                return Item();   // 成功放入背包，返回空物品指示“已收纳”
            }
            else
            {
                LOG("物品 {} 放入背包失败（背包可能已满），返回物品对象", item.getID());
            }
        }
    }
    return item;  // 未指定 owner 或背包不可用/满，返回物品对象供调用者进一步处理
}