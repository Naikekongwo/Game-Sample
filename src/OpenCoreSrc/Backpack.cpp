#include "OpenCore/Runtime/Gameplay/Backpack/Backpack.hpp"
#include "OpenCore/OpenCore.hpp"
#include <algorithm>
#include <cstddef>
#include <optional>

Backpack::Backpack(short capacity, short id)
    : BackpackID(id), BackpackCapacity(capacity)
{
    if (capacity <= 0)
    {
        LOG("创建背包失败：容量必须大于 0");
        throw std::runtime_error("Backpack capacity must be positive.");
    }

    slots_ = vector<ItemInstance>(capacity, {std::nullopt, 0});
}

void Backpack::resortBackpack()
{
    // 合并同类槽位
    for (auto it = slots_.begin(); it != slots_.end(); ++it)
    {
        if (!it->item.has_value())
            continue;

        for (auto jt = it + 1; jt != slots_.end();)
        {
            if (!jt->item.has_value())
            {
                ++jt;
                continue;
            }

            if (it->item->getTypeID() == jt->item->getTypeID())
            {
                uint8_t maxStack = ItemManager::getInstance().getMaxStackSize(
                    it->item->getTypeID());
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
    // 按 typeID 排序
    std::sort(slots_.begin(), slots_.end(),
              [](const ItemInstance &a, const ItemInstance &b)
              {
                  if (!a.item.has_value())
                      return false;
                  if (!b.item.has_value())
                      return true;
                  return a.item->getTypeID() < b.item->getTypeID();
              });
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

bool Backpack::addItem(short ItemTypeID, short ItemAmount)
{
    bool result = false;
    for (int index = 0; index < BackpackCapacity; index++)
    {
        if (setItem(ItemTypeID, ItemAmount, index))
        {
            result = true;
        }
    }
    return result;
}

bool Backpack::setItem(short ItemTypeID, short ItemAmount, short index)
{
    if (getCapacity() <= index)
    {
        return false;
    }

    if (slots_.at(index).count != 0 or slots_.at(index).item != std::nullopt)
    {
        return false;
    }

    slots_.at(index).item = Gameplay::ItemMgr.createItem(ItemTypeID);

    if (slots_.at(index).item == std::nullopt)
    {
        LOG("试图设置的物品ID {} 并没有得到注册！", ItemTypeID);
        return false;
    }

    slots_.at(index).count = ItemAmount;
    LOG("ID为{}的背包的索引{}处已经被加入了物品{}", BackpackID, index,
        ItemTypeID);
    return true;
}

std::optional<ItemInstance> Backpack::getItem(short index)
{
    if (index >= BackpackCapacity)
    {
        return std::nullopt;
    }

    return slots_.at(index);
}