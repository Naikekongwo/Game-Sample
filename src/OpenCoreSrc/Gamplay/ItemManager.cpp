#include "OpenCore/OpenCore.hpp"
#include "Union.hpp"

bool ItemManager::countItemById(uint16_t id)
{
    return (items_.count(id) > 0) ? true : false;
}

bool ItemManager::registerItem(std::shared_ptr<Item> item)
{
    // 检查ID是否存在
    if(countItemById(item->getID()))
    {
        return false;
    }
    items_[item->getID()] = item;
    return true;
}

bool ItemManager::deleteItem(uint16_t itemId)
{
    if(!countItemById(itemId))
    {
        return false;
    }
    return true;
}

std::shared_ptr<Item> ItemManager::modifyItem(uint16_t itemId)
{
    if(!countItemById(itemId))
    {
        return nullptr;
    }
    return items_[itemId];
}