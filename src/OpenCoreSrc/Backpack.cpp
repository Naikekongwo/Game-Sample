#include "OpenCore/Runtime/Gameplay/Backpack/Backpack.hpp"
#include "OpenCore/OpenCore.hpp"
#include <algorithm>

Backpack::Backpack(short capacity, short id)
    : BackpackID(id), BackpackCapacity(capacity)
{
    if (capacity <= 0)
    {
        LOG("创建背包失败：容量必须大于 0");
        throw std::runtime_error("Backpack capacity must be positive.");
    }
}

void Backpack::resortBackpack()
{
    // 合并同类槽位
    for (auto it = slots_.begin(); it != slots_.end(); ++it)
    {
        for (auto jt = it + 1; jt != slots_.end();)
        {
            if (it->item.getTypeID() == jt->item.getTypeID())
            {
                uint8_t maxStack = ItemManager::getInstance().getMaxStackSize(
                    it->item.getTypeID());
                uint8_t space = maxStack - it->count;
                if (space >= jt->count)
                {
                    it->count += jt->count;
                    jt = slots_.erase(jt);
                }
                else
                {
                    it->count = maxStack;
                    jt->count -= space;
                    ++jt;
                }
            }
            else
            {
                ++jt;
            }
        }
    }
    // 按 typeID 和 statueID 排序（可选）
    std::sort(slots_.begin(), slots_.end(),
              [](const ItemInstance &a, const ItemInstance &b)
              { return a.item.getTypeID() < b.item.getTypeID(); });
}

void Backpack::onUpdate(ItemExchangeRecord &record, float totalTime)
{
    // 框架：可根据具体交换逻辑完善
    if (record.srcBackpackID == this->BackpackID)
    {
        // 本地是给予方
    }
    else if (record.dstBackpackID == this->BackpackID)
    {
        // 本地是接收方
    }
}