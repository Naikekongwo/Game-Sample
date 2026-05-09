/**
 * @file PhysicalProperties.h
 * @author your name (you@domain.com)
 * @brief 物理系统的信息数据结构
 * @version 0.1
 * @date 2026-04-23
 *
 * @copyright Copyright (c) 2026
 *
 */

#pragma once

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

/**
 * @brief 物理信息类
 *
 */
class PhysicalProperties
{
  public:
    void onUpdate(float totalTime);

    // 新增：设置期望水平速度（世界x, y平面）
    void setDesiredVelocity(const Vec3& vel) { desiredVelocity = vel; }

    void setAccelParams(float maxAccel, float gain) {
        maxAccelParam = maxAccel;
        accelGain     = gain;
    }

    void setPosition(const Vec3 &position) { Position = position; }
    void setSpeed(const Vec3 &speed) { Speed = speed; }
    void setμFactor(const float &factor) { μFactor = factor; }

    void addSpeed(const Vec3 &speed)
    {
        Speed.x += speed.x;
        Speed.y += speed.y;
        Speed.z += speed.z;
    }

    Direction getDirection() const noexcept { return direction; }

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
    float μFactor = -0.6f;
    float lastTime = 0.0f;
    Vec3 Speed{0, 0, 0};
    Vec3 Position{0, 0, 0};
    Direction direction = Direction::Down;

    // 新增平滑加速相关
    Vec3 desiredVelocity{0,0,0};
    float maxAccelParam = 15.0f;   // 最大加速度（单位/s²）,控制起步/刹车的最大加速度，数值越大响应越灵敏，但过高会显得突跳,12.0 ~ 20.0
    float accelGain     = 10.0f;   // 趋近增益,接近目标速度时的柔和度，数值越大尾部越“硬”，过小则感觉迟滞,8.0 ~ 15.0
    float stopSmoothFactor = 10.0f; // 停止时向整数坐标平滑靠拢的速度，值越大越快

    void parseHorizontalMovement(float &Speed, float &Pos, float deltaTime);
    void parseVerticalMovement(float &Speed, float &Pos, float deltaTime);
    void applyMoveControl(float deltaTime);
    void smoothStop(float deltaTime);
};