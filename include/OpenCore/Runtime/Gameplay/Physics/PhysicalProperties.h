#pragma once

// PhysicalProperties
// 生物的物理属性

#include "OpenCore/Core/Math/OpenCore_Vec2.hpp"
#include "OpenCore/Core/Math/OpenCore_Vec3.hpp"
#include "OpenCore_Collision.hpp"

using Vec3 = OpenCore_Vec3;
using Vec2 = OpenCore_Vec2;

class PhysicalProperties
{
  public:
    void onUpdate(float totalTime);

    void setPosition(const Vec2 &position) { Position = position; }
    void giveSpeed(const Vec2 &speed)
    {
        Speed.x += speed.x;
        Speed.y += speed.y;
    }

    Vec2 getPosition() const { return Position; }

  private:
    // 物体的密度
    // 物体环境的摩擦力
    // 上一个时间节点
    // 物体的速度
    // 物体的位置
    // 物体的大小
    float density = 1.0f;
    float μFactor = 0.2f;
    float lastTime = 0.0f;
    Vec2 Speed{0, 0};
    Vec2 Position{0, 0};
};