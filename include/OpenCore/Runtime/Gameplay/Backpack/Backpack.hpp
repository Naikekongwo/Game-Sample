#pragma once

#include <vector>
#include <memory>
#include "Item.hpp"
#include "ItemManager.hpp"

using std::vector;
using std::shared_ptr;

// 单个背包槽位
struct BackpackSlot
{
    Item item;          // 该堆叠的代表物品（用于渲染等）
    uint8_t count = 1;  // 当前堆叠数量
};

// 物品交换记录（保留原设计，后续可配合 onUpdate 完善）
struct ItemExchangeRecord
{
    short ItemTypeID = 0;
    short statueID = 0;
    short ItemAmount = 0;

    short srcBackpackID = 0;
    short dstBackpackID = 0;

    bool dealed = false;
};

class Backpack final
{
public:
    explicit Backpack(short capacity, short id = 0);
    ~Backpack() = default;

    // 添加指定类型和状态的物品（数量可选）
    bool AddItem(short typeID, unsigned int statueID, uint8_t amount = 1);
    // 移除指定类型和状态的物品
    bool RemoveItem(short typeID, unsigned int statueID, uint8_t amount = 1);
    // 放入一个具体的 Item 对象（相当于 AddItem(item.getTypeID(), item.getStatueID(), 1)）
    bool pushItem(Item& item);

    // 整理背包：合并相同 type+statue 的相邻槽位，并排序（此处仅作示例）
    void resortBackpack();

    // ID 设置 / 获取
    void setBackpackID(short id) { BackpackID = id; }
    short getBackpackID() const noexcept { return BackpackID; }

    // 槽位数量
    size_t getSlotCount() const noexcept { return slots_.size(); }
    short getCapacity() const noexcept { return BackpackCapacity; }

    // 处理交换记录（基础框架）
    void onUpdate(ItemExchangeRecord &record, float totalTime);

private:
    // 查找相同 typeID 和 statueID 的槽位，返回下标，若无则返回 -1
    int findSlot(short typeID, unsigned int statueID) const;

    short BackpackID = 0;
    short BackpackCapacity = 0;
    vector<BackpackSlot> slots_;
};

using BackPtr = shared_ptr<Backpack>;