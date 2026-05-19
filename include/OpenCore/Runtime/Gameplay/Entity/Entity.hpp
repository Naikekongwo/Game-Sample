#pragma once

// Entity类
#include "OpenCore//Runtime//Graphics//Sprite//Mob.h"
#include "OpenCore/Runtime/Gameplay/Physics/PhysicalProperties.h"

#include "OpenCore/Runtime/Gameplay/Backpack/Backpack.hpp"

#include "EntityInfo.hpp"

#include <cstdint>
#include <memory>

#define ENTITY_PLAYER_ID 1

class Sprite;

using std::string;
using std::unique_ptr;

enum EntityStatus
{
    Registered,
    Drawable,
    Ready
};

struct EntityProperties
{
    PhysicalProperties *pProperties;
    EntityInfo *EntityInfo;
};

class Entity
{
  public:
    void Spawn(Vec3 Position);
    void onUpdate(float totalTime);
    void Configure(const EntityInfo &eInfo);
    bool Ready() const noexcept { return status >= drawable; }

    void Draw(float cameraX, float cameraY);
    void Draw(const Vec3 &absPos);
    void enableDrawer(bool enabled = true)
    {
        drawable = enabled;
        status = EntityStatus::Drawable;
    }
    void createRenderer();

    EntityInfo getEntityInfo() const noexcept { return info; }
    PhysicalProperties &getPhysicalProperties() { return pProperties; }
    BackPtr &getBackpack() { return backpack; }
    shared_ptr<float> getHealthHook() { return m_healthPercent; }
    /// @brief 检查实体在目标位置是否会与不可通行的瓦片碰撞
    bool canMoveTo(const Vec3 &pos) const;

    void setDesiredVelocity(const Vec3 &vel)
    {
        pProperties.setDesiredVelocity(vel);
    }
    void configMoveParams(float maxAccel, float gain)
    {
        pProperties.setAccelParams(maxAccel, gain);
    }

  private:
    string id;
    EntityInfo info;
    EntityStatus status = EntityStatus::Registered;

    PhysicalProperties pProperties;

    unique_ptr<Mob> renderer;
    bool drawable = false;
    float widthRelative = 1.0f;
    float heightRelative = 1.0f;
    uint16_t m_baseFrame = 0; ///< 精灵表中实体纹理的起始线性索引

    BackPtr backpack;

    shared_ptr<float> m_healthPercent;
    uint8_t maxHealth = 255;

    float lastTime = 0.0f;
};
