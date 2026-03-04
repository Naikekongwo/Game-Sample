#pragma once

// Entity类
#include "OpenCore/Runtime/Gameplay/Physics/PhysicalProperties.h"

#include "OpenCore//Runtime//Graphics//Sprite//Mob.h"

#include <memory>

class Sprite;

using std::string;
using std::unique_ptr;

class Entity
{
  public:
    void Spawn(Vec3 Position);
    void onUpdate(float totalTime) { pProperties.onUpdate(totalTime); }
    void Draw();

    void enableDrawer(bool enabled = true) { drawable = enabled; }

    void createRenderer();

  private:
    // 实体带有自我ID
    // 实体内置一个物理信息表
    // 实体的渲染类
    string id;
    PhysicalProperties pProperties;
    unique_ptr<Mob> renderer;
    bool drawable = false;
};