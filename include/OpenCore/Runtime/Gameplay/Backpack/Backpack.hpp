#pragma once

#include "Item.hpp"
#include "ItemManager.hpp"
#include <memory>
#include <vector>

using std::shared_ptr;
using std::vector;

// 单个背包槽位
struct ItemInstance
{
    Item item;         // 该堆叠的代表物品（用于渲染等）
    uint8_t count = 0; // 当前堆叠数量
};

// 物品交换记录（保留原设计，后续可配合 onUpdate 完善）
struct ItemExchangeRecord
{
    ItemInstance instance;

    short srcBackpackID = 0;
    short dstBackpackID = 0;

    bool dealed = false;
};

class Backpack final
{
  public:
    explicit Backpack(short capacity, short id = 0);
    ~Backpack() = default;

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
    short BackpackID = 0;
    short BackpackCapacity = 0;
    vector<ItemInstance> slots_;
};

using BackPtr = shared_ptr<Backpack>;