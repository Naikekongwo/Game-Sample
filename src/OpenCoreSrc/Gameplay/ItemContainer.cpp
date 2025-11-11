#include "OpenCore/OpenCore.hpp"
#include "Eclipsea/Eclipsea.hpp"

ItemContainer::ItemContainer()
{
    item_ = Gameplay::ItemMgr.getItem(0);
    // 初始化，自动为空物品
    amount_ = 0;
}

bool ItemContainer::transferItemIn(ItemContainer* src)
{
    if(!src) return false;

    src->transferItemTo(this);

    return true;
}

    
bool ItemContainer::transferItemTo(ItemContainer* dst)
{
    if(!dst) return false;

    Items item { item_, amount_ };

    if(dst->isEmpty() && dst->changeElement(item))
    {
        // 空且切换成功
        resetItem();
        return true;
    }

    return false;
}


bool ItemContainer::resetItem()
{
   item_.reset();
   
   amount_ = 0;

   return true;
}

bool ItemContainer::changeElement(Items item)
{
    if(item.first->getID() != 0 && item.second > 0)
    {
        item_ = item.first;
        amount_ = item.second;
    }
    return false;
}