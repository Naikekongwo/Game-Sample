#include "OpenCore/OpenCore.hpp"
#include <cmath>
void PhysicalProperties::onUpdate(float totalTime)
{
    float deltaTime = totalTime - lastTime;
    if (deltaTime <= 0.0f) {
        lastTime = totalTime;
        return;
    }

    // 1. 根据目标速度平滑加速/减速（水平）
    applyMoveControl(deltaTime);

    // 2. 垂直运动与落地检测
    parseVerticalMovement(Speed.z, Position.z, deltaTime);

    // 3. 摩擦力处理（两轴都处理，消除原单轴分支）
    parseHorizontalMovement(Speed.x, Position.x, deltaTime);
    parseHorizontalMovement(Speed.y, Position.y, deltaTime);

    // 4. 更新朝向
    if (Speed.x > 0.1f) direction = Direction::Right;
    else if (Speed.x < -0.1f) direction = Direction::Left;
    else if (Speed.y > 0.1f) direction = Direction::Down;
    else if (Speed.y < -0.1f) direction = Direction::Up;

    lastTime = totalTime;
}

// 处理平面移动的函数
void PhysicalProperties::parseHorizontalMovement(float &Speed, float &Pos,
                                                 float deltaTime)
{
    if (Speed != 0.0f)
    {
        bool negative = Speed < 0.0f;
        float deltaSpeed = 10.0f * μFactor * deltaTime;
        float futureSpeed = Speed + (negative ? -deltaSpeed : deltaSpeed);

        // 位移的计算依托于匀变速变形公式 V²₂ - V₁² = 2 * a * x
        float offset =
            (pow(futureSpeed, 2) - pow(Speed, 2)) / (2 * μFactor * 10.0f);
        if (negative)
            offset = -offset;

        // futureSpeed 是推断出来的速度
        // 这里的判断是判断速度是否反向
        // 摩擦力不可能使速度反向，所以如果反向则立即归零
        if ((futureSpeed < 0.0f && !negative) ||
            (futureSpeed > 0.0f && negative))
            Speed = 0.0f;
        else
            Speed = futureSpeed;

        Pos += offset;
    }
    else
    {
        // 若速度为0，则对齐
        Pos = std::round(Pos);
    }
}

void PhysicalProperties::parseVerticalMovement(float &Speed, float &Pos,
                                               float deltaTime)
{
    if (Speed != 0.0f or Position.z > 0.0f)
    {
        // 如果垂直速度不为0，或者坐标大于地面
        // 这是用于判断是否在空中的条件
        // 如果在空中，则利用动量定理
        float deltaSpeed = gravity * deltaTime;
        Pos += Speed * deltaTime + 0.5f * gravity * deltaTime * deltaTime;
        Speed += deltaSpeed;

        // 落地的检测
        if (Pos <= 0.0f)
        {
            Pos = 0.0f;
            Speed = 0.0f;
        }
    }
    else
    {
        Pos = 0.0f;
    }
}

void PhysicalProperties::applyMoveControl(float deltaTime)
{
    if (deltaTime <= 0.0f) return;

    // 仅处理水平面 (x, y)，保持垂直速度不变
    Vec3 target = desiredVelocity;
    target.z = 0.0f;

    auto applyAxis = [&](float cur, float tgt) -> float {
        float diff = tgt - cur;
        // 理想加速度 = 增益 * 误差
        float ideal = accelGain * diff;
        // 限制在最大加速度范围内
        float accel = std::clamp(ideal, -maxAccelParam, maxAccelParam);
        // 本帧速度变化量
        float delta = accel * deltaTime;
        // 避免超调：变化量不得超过剩余差值
        if (diff > 0.0f)
            delta = std::min(delta, diff);
        else if (diff < 0.0f)
            delta = std::max(delta, diff);
        return cur + delta;
    };

    Speed.x = applyAxis(Speed.x, target.x);
    Speed.y = applyAxis(Speed.y, target.y);
    // Speed.z 不动
}