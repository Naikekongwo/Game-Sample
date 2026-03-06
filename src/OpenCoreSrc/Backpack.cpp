#include "OpenCore/OpenCore.hpp"

bool Backpack::AddItem(short ItemTypeID, short ItemAmount)
{
    auto item = Gameplay::ItemMgr.createItem(ItemTypeID);

    if (item.getID() != ILEGAL_ITEM)
    {
        item.setAmount(ItemAmount);
        return pushItem(item);
    }
    return false;
}

// 删除特定的道具特定个
bool Backpack::RemoveItem(short ItemTypeID, short ItemAmount)
{
    for (int i = 0; i < itemPool_.size(); i++)
    {
        // 遍历
        if (itemPool_[i].getID() == ItemTypeID &&
            itemPool_[i].getAmount() >= ItemAmount)
        {
            return itemPool_[i].setAmount(itemPool_[i].getAmount() -
                                          ItemAmount);
        }
        else
        {
            return false;
        }
    }
    return false;
}

bool Backpack::pushItem(Item item)
{
    if (item.getID() == ILEGAL_ITEM)
        return false;

    if (itemPool_.size() + 1 > BackpackCapacity)
        return false;

    itemPool_.push_back(item);
    resortBackpack();
    return true;
}

void Backpack::resortBackpack()
{
    // 重排背包
}