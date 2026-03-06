#pragma once

// Backpack.hpp
// 控制实体携带的物品的类

#include <memory>
#include <stdexcept>
#include <vector>

using std::shared_ptr;
using std::vector;

#include "ItemManager.hpp"

// 物品交换记录
// 包含了物品的ID、数量、原始背包ID、目标背包ID和是否完成
struct ItemExchangeRecord
{
    short ItemTypeID = 0;
    short ItemAmount = 0;

    short srcBackpackID = 0;
    short dstBackpackID = 0;

    bool dealed = false;
};

class Backpack final
{
  public:
    // 初始化
    Backpack(short capacity) : BackpackCapacity(capacity)
    {
        if (capacity == 0)
        {
            throw std::runtime_error(
                "Backpack::~ Failed to create backpack, the capacity is null!");
            return;
        }
    }

    void onUpdate(ItemExchangeRecord &record, float totalTime)
    {
        if (record.srcBackpackID == this->BackpackID)
        {
            // 此为给予方
        }
        else if (record.dstBackpackID == this->BackpackID)
        {
            // 此为接收方
        }
        else
        {
        }
    }

    bool AddItem(short ItemTypeID, short ItemAmount);
    bool RemoveItem(short ItemTypeId, short ItemAmount);
    bool pushItem(Item item);
    void resortBackpack();

    void setBackpackID(short ID) { BackpackID = ID; }
    short getBackpackID() const noexcept { return BackpackID; }

  private:
    short BackpackID = 0;
    vector<Item> itemPool_;
    short BackpackCapacity = 0;
};

using BackPtr = shared_ptr<Backpack>;