#include "Eclipsea/Gameplay/Backpack/Item.hpp"
#include "Core/Math/OpenCore_Vec3.hpp"
#include "OpenCore.hpp"
#include "Eclipsea/Gameplay/Entity/Entity.hpp"
#include "Eclipsea/Gameplay/Sprite/ItemSprite.hpp"
#include <cmath>

Item::Item(const ItemInfo &itemInfo)
{
    // 使用itemInfo进行构造的方法
    info = itemInfo;
    LOG("物品配置成功，类型ID：{}", info.typeID);
    status = ItemStatus::Ready;
}