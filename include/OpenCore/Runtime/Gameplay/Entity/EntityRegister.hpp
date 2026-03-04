#pragma once

// EntityRegister.hpp
// 注册 Entity 的注册器

#include "Entity.hpp"

#include <algorithm>
#include <memory>
#include <unordered_map>

using std::unordered_map;

using EntityPtr = std::unique_ptr<Entity>;

class EntityRegister
{
  public:
    //   单例模式
    static EntityRegister &getInstance()
    {
        static EntityRegister instance;
        return instance;
    }

    bool registerEntity(const EntityInfo &eInfo);
    bool containsEntity(short EntityTypeID) const noexcept
    {
        return EntityList.contains(EntityTypeID);
    }

    EntityPtr createEntity(short EntityTypeID);
    EntityInfo getEntityInfo(short EntityTypeID);

  private:
    unordered_map<short, EntityInfo> EntityList;
};