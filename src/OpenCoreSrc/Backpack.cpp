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

int Backpack::findSlot(short typeID, unsigned int statueID) const
{
    for (size_t i = 0; i < slots_.size(); ++i)
    {
        if (slots_[i].item.getTypeID() == typeID &&
            slots_[i].item.getStatueID() == statueID)
        {
            return static_cast<int>(i);
        }
    }
    return -1;
}

bool Backpack::AddItem(short typeID, unsigned int statueID, uint8_t amount)
{
    if (amount == 0) return true;  // 添加 0 个视为成功

    // 获取该类型物品的最大堆叠数（未注册则返回 64 作为默认值）
    uint8_t maxStack = ItemManager::getInstance().getMaxStackSize(typeID);

    // 先尝试合并已有槽位
    int idx = findSlot(typeID, statueID);
    if (idx != -1)
    {
        BackpackSlot &slot = slots_[idx];
        uint8_t space = maxStack - slot.count;
        if (space >= amount)
        {
            slot.count += amount;
            return true;
        }
        else
        {
            // 填满当前槽位，剩余部分作为新槽位
            slot.count = maxStack;
            amount -= space;
        }
    }

    // 需要新槽位
    while (amount > 0 && slots_.size() < static_cast<size_t>(BackpackCapacity))
    {
        // 创建一个代表物品（通过 ItemManager，owner 为 nullptr）
        Item newItem = ItemManager::getInstance().createItem(typeID, nullptr, statueID);
        if (newItem.getID().empty())  // 创建失败
        {
            LOG("创建物品实例失败，类型ID：{}", typeID);
            return false;
        }

        uint8_t toAdd = (amount > maxStack) ? maxStack : amount;
        BackpackSlot newSlot{ std::move(newItem), toAdd };
        slots_.push_back(std::move(newSlot));
        amount -= toAdd;
    }

    if (amount > 0)
    {
        LOG("背包已满，未放入数量：{}", amount);
        return false;
    }
    return true;
}

bool Backpack::RemoveItem(short typeID, unsigned int statueID, uint8_t amount)
{
    if (amount == 0) return true;

    int idx = findSlot(typeID, statueID);
    if (idx == -1)
    {
        LOG("未找到要移除的物品，类型：{}，状态：{}", typeID, statueID);
        return false;
    }

    BackpackSlot &slot = slots_[idx];
    if (slot.count < amount)
    {
        LOG("物品数量不足，现有：{}，需移除：{}", slot.count, amount);
        return false;
    }

    slot.count -= amount;
    if (slot.count == 0)
    {
        slots_.erase(slots_.begin() + idx);
    }
    return true;
}

bool Backpack::pushItem(Item& item)
{
    short typeID = item.getTypeID();
    unsigned int statueID = item.getStatueID();
    uint8_t maxStack = ItemManager::getInstance().getMaxStackSize(typeID);

    // 尝试合并已有槽位
    int idx = findSlot(typeID, statueID);
    if (idx != -1)
    {
        BackpackSlot& slot = slots_[idx];
        if (slot.count < maxStack)
        {
            ++slot.count;
            // 物品被吸收，移走其数据（可选，也可以保留但无意义）
            item = Item();      // 将 item 置为空，表示所有权转移
            return true;
        }
    }

    // 新增槽位
    if (slots_.size() < static_cast<size_t>(BackpackCapacity))
    {
        // 移动构造 BackpackSlot，从而移动 item 的所有资源
        slots_.emplace_back(BackpackSlot{ std::move(item), 1 });
        // 此时 item 处于有效但未指定状态（renderer 等被移走）
        return true;
    }
    return false; // 背包满
}
void Backpack::resortBackpack()
{
    // 合并同类槽位
    for (auto it = slots_.begin(); it != slots_.end(); ++it)
    {
        for (auto jt = it + 1; jt != slots_.end(); )
        {
            if (it->item.getTypeID() == jt->item.getTypeID() &&
                it->item.getStatueID() == jt->item.getStatueID())
            {
                uint8_t maxStack = ItemManager::getInstance().getMaxStackSize(it->item.getTypeID());
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
        [](const BackpackSlot &a, const BackpackSlot &b) {
            if (a.item.getTypeID() != b.item.getTypeID())
                return a.item.getTypeID() < b.item.getTypeID();
            return a.item.getStatueID() < b.item.getStatueID();
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