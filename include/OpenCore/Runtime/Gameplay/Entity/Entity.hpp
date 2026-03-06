#pragma once

// Entity类
#include "OpenCore//Runtime//Graphics//Sprite//Mob.h"
#include "OpenCore/Runtime/Gameplay/Physics/PhysicalProperties.h"

#include "EntityInfo.hpp"

#include <memory>

class Sprite;

using std::string;
using std::unique_ptr;

class Entity
{
  public:
    void Spawn(Vec3 Position);
    void onUpdate(float totalTime);
    void Draw(float cameraX, float cameraY);
    void enableDrawer(bool enabled = true) { drawable = enabled; }
    void createRenderer();

    void Configure(const EntityInfo &eInfo);

    PhysicalProperties &getPhysicalProperties() { return pProperties; }

  private:
    // 实体带有自我ID
    // 实体内置一个物理信息表
    // 实体的渲染类
    string id;
    PhysicalProperties pProperties;
    unique_ptr<Mob> renderer;
    bool drawable = false;
    EntityInfo info;

    // 渲染参数
    float widthRelative = 1.0f;
    float heightRelative = 1.0f;
};