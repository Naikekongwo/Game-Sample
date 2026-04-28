#pragma once

// ItemManager.hpp
// 物品注册与创建管理器

#include <memory>
#include <unordered_map>
#include <string>
#include "OpenCore/Runtime/Gameplay/Backpack/Item.hpp"

using std::unordered_map;
using std::string;

// 物品注册信息（仅用于注册表）
struct ItemRegInfo
{
    short typeID = 0;                  // 物品类型 ID
    int textureID = 0;                 // 纹理 ID
    float widthFactor = 1.0f;         // 缩放因子
    uint8_t maxAmount = 64;            // 最大堆叠数量（供背包等系统参考）
    unsigned int allocatedCount = 0;   // 该类型已分配实例数量，用于自动生成唯一 ID
};

class ItemManager final
{
public:
    static ItemManager &getInstance()
    {
        static ItemManager instance;
        return instance;
    }

    uint8_t getMaxStackSize(short typeID) const;
    // 注册物品类型
    bool registerItem(const ItemRegInfo &regInfo);

    // 创建物品实例（自动生成唯一实例 ID）。
    // 如果 owner 非空，则尝试将物品放入该实体的背包；若放入成功，返回空物品（调用者可据此判断是否已入包）；
    // 若放入失败（例如背包满），返回一个已配置好的物品，可供其他处理（如丢弃到地面）。
    Item createItem(short typeID, 
                    Entity* owner = nullptr, 
                    unsigned int statueID = 0);

private:
    ItemManager() = default;

    // 将注册信息与运行时参数合并为完整的 ItemInfo
    ItemInfo buildItemInfo(ItemRegInfo &regInfo,    // 注意传入引用以修改计数器
                           Entity* owner,
                           unsigned int statueID);

    unordered_map<short, ItemRegInfo> itemRegistry;  // 类型 ID -> 注册信息
};