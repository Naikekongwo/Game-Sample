#pragma once

// ItemContainerCollection.hpp
// 物品栏的实体

#include "UIElement.hpp"
#include "ItemContainer.hpp"

// 用作测试的ITEM类!

class Item
{
    public:
    Item(short id, short texID) : id(id), texID(texID) {};

    private:
    
    short id;
    short texID;
};

class ItemContainerCollection : public UIElement
{
    public:
    
    ItemContainerCollection(std::string id, short layer, short texID, short rows, short cols);



    void pushItem(Item* item, short slotID);

    void onUpdate(float totalTime) override;

    void onRender() override;

    protected:

    bool GroupIsReady = false;

    short texID = 2010;
    
    // 行,列数目
    short rows;
    short cols;

    std::vector<std::unique_ptr<ItemContainer>> ItemGroups;
};