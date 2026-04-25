#include "OpenCore/OpenCore.hpp"
#include <memory>

bool EntityRegister::registerEntity(const EntityInfo &eInfo)
{
    // 注册
    if (containsEntity(eInfo.EntityTypeID))
    {
        LOG("EntityRegister::registerEntity failed to register 'cas "
            "there's already one.");
        return false;
    }

    EntityList[eInfo.EntityTypeID] = eInfo;
    // 注册成功

    LOG("EntityRegister::registerEntity Entity type ID : {} was "
        "successfully registered.",
        eInfo.EntityTypeID);

    return true;
}

EntityPtr EntityRegister::createEntity(short EntityTypeID)
{
    auto entity = std::make_unique<Entity>();

    // 检查是否已经注册
    if (!containsEntity(EntityTypeID))
    {
        LOG("EntityRegister::createEntity() failed to create entity "
            "because it refers to a entity info that not exists.");
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