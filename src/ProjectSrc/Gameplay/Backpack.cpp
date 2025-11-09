#include "OpenCore/OpenCore.hpp"
#include "Union.hpp"
#include <algorithm>

uint16_t Backpack::countItem(uint16_t itemId) const
{
    uint16_t result = 0;

    // 使用基于范围的循环，更简洁
    for (const auto& slot : storage_) {
        if (slot.first->getID() == itemId) {
            result += slot.second;
        }
    }
    return result;
}

int Backpack::AddItem(Items item)
{
    // 参数检查
    if (!item.first || item.second == 0) {
        return item.second; // 无效物品，返回原数量
    }

    auto id = item.first->getID();
    auto maxStack = item.first->getMaxSize();
    uint16_t leftover = item.second;

    // 如果物品已存在，先尝试堆叠
    uint16_t currentAmount = countItem(id);
    if (currentAmount > 0) {
        // 遍历所有同ID物品槽位
        for (auto& slot : storage_) {
            if (slot.first->getID() == id && slot.second < maxStack) {
                uint16_t availableSpace = maxStack - slot.second;
                uint16_t addAmount = (leftover < availableSpace) ? leftover : availableSpace;
                
                slot.second += addAmount;
                leftover -= addAmount;

                if (leftover == 0) {
                    return 0; // 完全添加成功
                }
            }
        }
    }

    // 需要创建新槽位
    while (leftover > 0 && storage_.size() < capacity_) {
        uint16_t addAmount = (leftover > maxStack) ? maxStack : leftover;
        storage_.push_back({item.first, addAmount});
        leftover -= addAmount;
    }

    return leftover; // 返回剩余无法添加的数量
}

bool Backpack::RemoveItem(Items item)
{
    // 参数检查
    if (!item.first || item.second == 0) {
        return false;
    }

    auto id = item.first->getID();
    uint16_t toRemove = item.second;

    // 检查总量是否足够
    if (countItem(id) < toRemove) {
        return false;
    }

    // 从后往前移除，避免迭代器失效
    for (auto it = storage_.rbegin(); it != storage_.rend(); ) {
        if (it->first->getID() == id) {
            if (it->second <= toRemove) {
                toRemove -= it->second;
                // 反向迭代器删除需要特殊处理
                it = std::reverse_iterator(storage_.erase(std::next(it).base()));
            } else {
                it->second -= toRemove;
                return true; // 移除完成
            }
        } else {
            ++it;
        }

        if (toRemove == 0) {
            return true; // 移除完成
        }
    }

    return true;
}