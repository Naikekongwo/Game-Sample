#include "OpenCore/OpenCore.hpp"
#include <cmath>

void PhysicalProperties::onUpdate(float totalTime)
{
    float deltaTime = totalTime - lastTime;
    parseVerticalMovement(Speed.z, Position.z, deltaTime);

    if (Speed.x != 0.0f)
    {
        parseHorizontalMovement(Speed.x, Position.x, deltaTime);
    }
    else
        parseHorizontalMovement(Speed.y, Position.y, deltaTime);

    // 检测速度
    if (Speed.y > 0.0f)
        direction = Direction::Down;
    if (Speed.y < 0.0f)
        direction = Direction::Up;
    if (Speed.x > 0.0f)
        direction = Direction::Right;
    if (Speed.x < 0.0f)
        direction = Direction::Left;
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