#include "OpenCore/OpenCore.hpp"
#include <cmath>
void PhysicalProperties::onUpdate(float totalTime)
{
    float deltaTime = totalTime - lastTime;
    if (deltaTime <= 0.0f) {
        lastTime = totalTime;
        return;
    }

    bool hasInput = (desiredVelocity.x != 0.0f || desiredVelocity.y != 0.0f);
    if (hasInput)
    {
        // 有输入：平滑加速到目标速度
        applyMoveControl(deltaTime);
    }
    else
    {
        // 无输入：平滑靠拢到运动方向上的整数坐标
        smoothStop(deltaTime);
    }

    // 垂直运动与落地检测
    parseVerticalMovement(Speed.z, Position.z, deltaTime);

    // 更新朝向
    if (Speed.x > 0.1f) direction = Direction::Right;
    else if (Speed.x < -0.1f) direction = Direction::Left;
    else if (Speed.y > 0.1f) direction = Direction::Down;
    else if (Speed.y < -0.1f) direction = Direction::Up;

    lastTime = totalTime;
}

// 处理平面移动的函数（保留但不再被 onUpdate 调用）
void PhysicalProperties::parseHorizontalMovement(float &Speed, float &Pos,
                                                 float deltaTime)
{
    if (Speed != 0.0f)
    {
        bool negative = Speed < 0.0f;
        float deltaSpeed = 10.0f * μFactor * deltaTime;
        float futureSpeed = Speed + (negative ? -deltaSpeed : deltaSpeed);

        float offset =
            (pow(futureSpeed, 2) - pow(Speed, 2)) / (2 * μFactor * 10.0f);
        if (negative)
            offset = -offset;

        if ((futureSpeed < 0.0f && !negative) ||
            (futureSpeed > 0.0f && negative))
            Speed = 0.0f;
        else
            Speed = futureSpeed;

        Pos += offset;
    }
    else
    {
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

    Position.x += Speed.x * deltaTime;
    Position.y += Speed.y * deltaTime;
}

void PhysicalProperties::smoothStop(float deltaTime)
{
    auto stopAxis = [&](float &speed, float &pos) {
        if (speed == 0.0f) return;

        float target = (speed > 0.0f) ? std::ceil(pos) : std::floor(pos);
        float remaining = target - pos;

        if (std::abs(remaining) < 0.001f) {
            pos = target;
            speed = 0.0f;
            return;
        }

        // a = -v² / (2d) : 恰好停在 target 所需的恒定加速度
        float required = -(speed * speed) / (2.0f * remaining);
        float accel = std::clamp(required, -stopSmoothFactor, stopSmoothFactor);

        float prevSpeed = speed;
        speed += accel * deltaTime;

        // 防止速度过零回弹
        if ((prevSpeed > 0.0f && speed < 0.0f) ||
            (prevSpeed < 0.0f && speed > 0.0f))
            speed = 0.0f;

        pos += speed * deltaTime;

        // 防止离散积分导致的位置越过目标
        if ((prevSpeed > 0.0f && pos > target) ||
            (prevSpeed < 0.0f && pos < target)) {
            pos = target;
            speed = 0.0f;
        }
    };

    stopAxis(Speed.x, Position.x);
    stopAxis(Speed.y, Position.y);
}