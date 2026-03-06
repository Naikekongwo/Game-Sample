#include "OpenCore/OpenCore.hpp"

Item ItemManager::createItem(short ItemTypeID)
{
    if (!ItemInfos.contains(ItemTypeID))
        return emptyItem;

    // 正常生成Item

    auto itemInfo = ItemInfos.at(ItemTypeID);

    Item item(itemInfo.ItemTypeID, itemInfo.ItemMaxSize);
    // 处理 ItemInfo

    return item;
}