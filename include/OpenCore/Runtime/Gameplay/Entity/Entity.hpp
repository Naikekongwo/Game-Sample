#pragma once

// Entity类
#include "OpenCore//Runtime//Graphics//Sprite//Mob.h"
#include "OpenCore/Runtime/Gameplay/Physics/PhysicalProperties.h"

#include "OpenCore/Runtime/Gameplay/Backpack/Backpack.hpp"

#include "EntityInfo.hpp"

#include <cstdint>
#include <memory>
#include <unordered_map>

#define ENTITY_PLAYER_ID 1

class Sprite;

using std::string;
using std::unique_ptr;
using std::unordered_map;

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

    // ─── 动画状态 ───────────────────────────────────
    unordered_map<short, AnimationGroup> m_animMap; // id → 动画组
    short m_currentAnimId = 0;                      // 当前播放 id，0=idle
    uint8_t m_currentFrame = 0;                     // 当前帧序号
    float m_frameTimer = 0.0f;                      // 帧计时器（秒）

    /// @brief Direction → animId 映射（Up=1, Down=2, Left=3, Right=4）
    static short directionToAnimId(Direction dir);

    /// @brief 返回当前应显示的帧索引（动画帧或 idle 帧）
    uint16_t getCurrentFrameIndex() const;
};
