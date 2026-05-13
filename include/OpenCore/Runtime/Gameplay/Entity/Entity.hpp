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
    void Draw(float cameraX, float cameraY);

    void Draw(const Vec3 &absPos);
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

    // 提供给输入系统的接口
    void setDesiredVelocity(const Vec3 &vel)
    {
        pProperties.setDesiredVelocity(vel);
    }

    // 也可保留一个配置接口供蓝图/调试使用
    void configMoveParams(float maxAccel, float gain)
    {
        pProperties.setAccelParams(maxAccel, gain);
    }

    shared_ptr<float> getHealthHook() { return m_healthPercent; }

    /// @brief 检查实体在目标位置是否会与不可通行的瓦片碰撞
    bool canMoveTo(const Vec3 &pos) const;

    /// @brief 切换当前动画序列
    void setAnimIndex(uint8_t index) { m_currentAnimIndex = index; }
    uint8_t getAnimIndex() const { return m_currentAnimIndex; }

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

    shared_ptr<float> m_healthPercent;
    uint8_t maxHealth = 255;

    // 渲染参数
    float widthRelative = 1.0f;
    float heightRelative = 1.0f;

    float lastTime = 0.0f;

    /// @brief 将 Direction 映射为贴图方向块的行偏移乘数 (Down=0, Left=1, Right=2, Up=3)
    static int directionBaseIndex(Direction dir, int tilesPerDirection);

    float m_animTimer = 0.0f;           // 动画帧计时器
    float m_animFPS = 6.0f;             // 动画播放速率
    uint8_t m_currentAnimIndex = 0;     // 当前动画序列索引
    uint8_t m_gridCols = 4;             // 精灵表网格列数

    /// @brief 根据当前状态（静止/移动方向）计算应播放的动画序列索引
    uint8_t computeAnimIndex() const;
};