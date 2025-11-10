#ifndef _ITEMCONTAINER_H_
#define _ITEMCONTAINER_H_

// ItemContainer.hpp
// 存放Item的数据结构

#include "OpenCore/Gameplay/Items/Item.hpp"

class ItemContainer
{
    public:

    ItemContainer();

    bool isEmpty() const { return amount_; }

    std::string getItemName() const { return item_->getName(); }

    uint16_t getItemId() const { return item_->getID(); }

    bool transferItemIn(ItemContainer* src);

    bool transferItemTo(ItemContainer* dst);

    bool changeElement(Items item);

    bool resetItem();


    private:

    // 定义储存在此处的Item的具体类型
    std::shared_ptr<Item> item_;
    // 定义个数
    uint8_t amount_;

};

#endif //_ITEMCONTAINER_H_