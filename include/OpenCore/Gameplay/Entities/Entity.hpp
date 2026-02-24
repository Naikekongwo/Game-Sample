#pragma once

// Entity.hpp
// 实体的基类

class PhysicalProperties
{
  public:
    PhysicalProperties() = default;

    void getAbsolutePosition(float &x, float &y)
    {
        x = absolute_X;
        y = absolute_Y;
    }

    void getRelativePostion(const PhysicalProperties &other, float &x, float &y)
    {
        x = absolute_X - other.absolute_X;
        y = absolute_Y - other.absolute_Y;
    }

  private:
    // 绝对的X，Y，速度
    float absolute_X = 0.0f;
    float absolute_Y = 0.0f;
    float absolute_Speed[2]{0.0f, 0.0f};
};

class Entity
{
  public:
    Entity() = default;

    PhysicalProperties getProperties() const { return pProperties; }

    // 实体的物理属性信息
    PhysicalProperties pProperties;
};