#pragma once

// PhysicalProperties
// 生物的物理属性

#include "OpenCore/Core/Math/OpenCore_Vec2.hpp"
#include "OpenCore/Core/Math/OpenCore_Vec3.hpp"
#include "OpenCore_Collision.hpp"

using Vec3 = OpenCore_Vec3;
using Vec2 = OpenCore_Vec2;

enum class Direction
{
    Up,
    Down,
    Left,
    Right
};

// 定义
// Position的一个单位为1m

static float gravity = -10.0f;

class PhysicalProperties
{
  public:
    void onUpdate(float totalTime);

    void setPosition(const Vec3 &position) { Position = position; }
    void setSpeed(const Vec3 &speed) { Speed = speed; }
    void setμFactor(const float &factor) { μFactor = factor; }

    void addSpeed(const Vec3 &speed)
    {
        Speed.x += speed.x;
        Speed.y += speed.y;
        Speed.z += speed.z;
    }

    Vec3 getPosition() const { return Position; }
    Vec3 getSpeed() const { return Speed; }
    float getμFactor() const { return μFactor; }

  private:
    // 物体的密度
    // 物体环境的摩擦力
    // 上一个时间节点
    // 物体的速度
    // 物体的位置
    // 物体的朝向
    float density = 1.0f;
    float μFactor = 0.2f;
    float lastTime = 0.0f;
    Vec3 Speed{0, 0, 0};
    Vec3 Position{0, 0, 0};
    Direction direction = Direction::Down;

    // 物理函数
    void parseHorizontalMovement(float &Speed, float &Pos, float deltaTime);
    void parseVerticalMovement(float &Speed, float &Pos, float deltaTime);
};