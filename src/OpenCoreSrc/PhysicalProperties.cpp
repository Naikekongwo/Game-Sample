#include "OpenCore/OpenCore.hpp"
#include <cmath>
void PhysicalProperties::onUpdate(float totalTime)
{
    float deltaTime = totalTime - lastTime;
    if (deltaTime <= 0.0f) {
        lastTime = totalTime;
        return;
    }

    // 逐轴独立处理：有输入 → 加速，无输入 → 平滑靠拢整数坐标
    auto processAxis = [&](float &speed, float &pos, float desired) {
        if (desired != 0.0f) {
            // ---- 加速到目标速度 ----
            float diff = desired - speed;
            float ideal = accelGain * diff;
            float accel = std::clamp(ideal, -maxAccelParam, maxAccelParam);
            float delta = accel * deltaTime;
            if (diff > 0.0f)
                delta = std::min(delta, diff);
            else if (diff < 0.0f)
                delta = std::max(delta, diff);
            speed += delta;
            pos += speed * deltaTime;
        } else if (speed != 0.0f) {
            // ---- 平滑靠拢到运动方向上的整数坐标 ----
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
        }
    };

    processAxis(Speed.x, Position.x, desiredVelocity.x);
    processAxis(Speed.y, Position.y, desiredVelocity.y);

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

