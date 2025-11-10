#include "OpenCore/OpenCore.hpp"
#include "Eclipsea/Eclipsea.hpp"

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

std::shared_ptr<Item> ItemManager::getItem(uint16_t itemId)
{
    return items_[itemId];
}

std::shared_ptr<Item> ItemManager::getItem(const std::string& name)
{
    for( auto it = items_.begin(); it != items_.end(); ++it)
    {
        if( it->second->getName() == name)
        {
            return items_[it->first];
        }
    }
    return nullptr;
}

ItemManager::ItemManager()
{
    auto emptyItem = std::make_shared<Item>(0, "emptyItem", nullptr);

    items_[0] = emptyItem;
}