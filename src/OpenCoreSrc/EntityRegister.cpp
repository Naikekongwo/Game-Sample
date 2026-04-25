#include "OpenCore/OpenCore.hpp"
#include <memory>

bool EntityRegister::registerEntity(const EntityInfo &eInfo)
{
    // 注册
    if (containsEntity(eInfo.EntityTypeID))
    {
        LOG("发现重复的实体词条，注册失败");
        return false;
    }

    EntityList[eInfo.EntityTypeID] = eInfo;
    // 注册成功

    LOG("注册成功，实体类型: {}", eInfo.EntityTypeID);

    return true;
}

EntityPtr EntityRegister::createEntity(short EntityTypeID)
{
    auto entity = std::make_unique<Entity>();

    // 检查是否已经注册
    if (!containsEntity(EntityTypeID))
    {
        LOG("创建实体失败，该EntityTypeID指向一个未注册或者不存在的实体, "
            "实体类型ID:{}",
            EntityTypeID);
        return nullptr;
    }
    // 开始配置Entity
    entity->Configure(EntityList.at(EntityTypeID));
    return entity;
}

// 从注册器中读取某个ID对应的EntityInfo
EntityInfo EntityRegister::getEntityInfo(short EntityTypeID)
{
    EntityInfo info;

    if (EntityList.contains(EntityTypeID))
        info = EntityList.at(EntityTypeID);

    return info;
}