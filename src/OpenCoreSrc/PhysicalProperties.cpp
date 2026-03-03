#include "OpenCore/OpenCore.hpp"
#include <cmath>

void PhysicalProperties::onUpdate(float totalTime)
{
    float deltaTime = totalTime - lastTime;

    auto applyFriction = [&](float &speed, float &pos) -> void
    {
        if (speed != 0.0f)
        {
            bool negative = speed < 0.0f;
            float deltaSpeed = 10.0f * μFactor * deltaTime;
            float likeSpeed = speed - (negative ? -deltaSpeed : deltaSpeed);

            // 基于匀变速公式计算位移 s = (v1^2 - v0^2) / (2a)
            float offset =
                (pow(likeSpeed, 2) - pow(speed, 2)) / (2 * μFactor * 10.0f);
            if (negative)
                offset = -offset;

            // 更新速度
            if ((likeSpeed < 0.0f && !negative) ||
                (likeSpeed > 0.0f && negative))
                speed = 0.0f;
            else
                speed = likeSpeed;

            pos += offset;
        }
        else
        {
            // 速度为 0 时，像素对齐
            pos = std::round(pos);
        }
    };

    if (Speed.x != 0.0f)
    {
        applyFriction(Speed.x, Position.x);
    }
    else
        applyFriction(Speed.y, Position.y);

    lastTime = totalTime;
}