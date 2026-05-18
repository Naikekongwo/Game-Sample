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
#include <utility>
#include <vector>

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

    /// @brief 设置实体占据的瓦片尺寸
    void setTileSize(uint8_t w, uint8_t h) { tileWidth = w; tileHeight = h; }
    uint8_t getTileWidth() const { return tileWidth; }
    uint8_t getTileHeight() const { return tileHeight; }

    /// @brief 设置碰撞高度缩放（0~1），Y 轴只取下半部分有效行
    void setCollisionHeightScale(float s) { collisionHeightScale = s; }
    float getCollisionHeightScale() const { return collisionHeightScale; }

    /// @brief 返回有效碰撞行数 = max(1, floor(tileHeight * collisionHeightScale))
    uint8_t getEffectiveTileHeight() const
    {
        int h = static_cast<int>(tileHeight * collisionHeightScale);
        return static_cast<uint8_t>(h > 0 ? h : 1);
    }

    /// @brief 获取实体当前位置下占据的所有瓦片坐标 (gx, gy)
    std::vector<std::pair<int, int>> getOccupiedTiles() const;

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

    uint8_t tileWidth = 1;   // 实体占几格宽（世界单位）
    uint8_t tileHeight = 1;  // 实体占几格高（世界单位）
    float collisionHeightScale = 1.0f; // 碰撞高度比例（Y轴有效占比，默认1.0=全部）

    void parseHorizontalMovement(float &Speed, float &Pos, float deltaTime);
    void parseVerticalMovement(float &Speed, float &Pos, float deltaTime);
};