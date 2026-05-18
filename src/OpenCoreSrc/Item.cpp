#include "OpenCore/Runtime/Gameplay/Backpack/Item.hpp"
#include "OpenCore/Core/Math/OpenCore_Vec3.hpp"
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Gameplay/Entity/Entity.hpp"
#include "OpenCore/Runtime/Graphics/Sprite/ItemSprite.hpp"
#include <cmath>

Item::Item(const ItemInfo &itemInfo)
{
    // 使用itemInfo进行构造的方法
    info = itemInfo;
    LOG("物品配置成功，类型ID：{}", info.typeID);
    status = ItemStatus::Ready;
}