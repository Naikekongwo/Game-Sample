#pragma once

// Entity类
#include "OpenCore//Runtime//Graphics//Sprite//Mob.h"
#include "OpenCore/Runtime/Gameplay/Physics/PhysicalProperties.h"

#include "OpenCore/Runtime/Gameplay/Backpack/Backpack.hpp"

#include "EntityInfo.hpp"

#include <memory>

class Sprite;

using std::string;
using std::unique_ptr;

enum EntityStatus
{
    Registered,
    Drawable,
    Ready
};

class Entity
{
  public:
    void Spawn(Vec3 Position);
    void onUpdate(float totalTime);
    void Draw(float cameraX, float cameraY);
    void enableDrawer(bool enabled = true)
    {
        drawable = enabled;
        status = EntityStatus::Drawable;
    }
    void createRenderer();

    void Configure(const EntityInfo &eInfo);

    bool Ready() const noexcept { return status >= drawable; }

    PhysicalProperties &getPhysicalProperties() { return pProperties; }
    BackPtr &getBackpack() { return backpack; }

  private:
    // 实体带有自我ID
    // 实体内置一个物理信息表
    // 实体的渲染类
    string id;
    PhysicalProperties pProperties;
    unique_ptr<Mob> renderer;
    bool drawable = false;
    EntityInfo info;

    // 背包
    BackPtr backpack;

    // 状态
    EntityStatus status = EntityStatus::Registered;

    // 渲染参数
    float widthRelative = 1.0f;
    float heightRelative = 1.0f;
};