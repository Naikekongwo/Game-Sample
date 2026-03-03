#pragma once

// OpenCore_Vec2
// 二维向量

#include <cmath>
#include <stdexcept>
#include <vector>

class OpenCore_Vec2
{
  public:
    float x, y;

    // 构造函数
    OpenCore_Vec2() : x(0.0f), y(0.0f) {}
    OpenCore_Vec2(float val) : x(val), y(val) {}
    OpenCore_Vec2(float x_, float y_) : x(x_), y(y_) {}

    // 加减
    OpenCore_Vec2 operator+(const OpenCore_Vec2 &other) const
    {
        return OpenCore_Vec2(x + other.x, y + other.y);
    }

    OpenCore_Vec2 operator-(const OpenCore_Vec2 &other) const
    {
        return OpenCore_Vec2(x - other.x, y - other.y);
    }

    // 等于
    bool operator==(const OpenCore_Vec2 &other) const
    {
        return (x == other.x && y == other.y);
    }

    // 点乘（Dot Product） - 返回标量
    float dot(const OpenCore_Vec2 &other) const
    {
        return x * other.x + y * other.y;
    }

    // 返回单位向量
    OpenCore_Vec2 normalized() const
    {
        float len = sqrt(x * x + y * y);
        if (len == 0.0f)
            return OpenCore_Vec2(0.0f, 0.0f);
        return OpenCore_Vec2(x / len, y / len);
    }

    // 将自身单位化
    OpenCore_Vec2 &normalize()
    {
        float len = sqrt(x * x + y * y);
        if (len != 0.0f)
        {
            x /= len;
            y /= len;
        }
        return *this;
    }

    float getLen() { return sqrt(x * x + y * y); }

    // 索引访问
    float &operator[](int index)
    {
        switch (index)
        {
        case 0:
            return x;
        case 1:
            return y;
        default:
            throw std::out_of_range("Vec2 index out of range.");
        }
    }

    const float &operator[](int index) const
    {
        switch (index)
        {
        case 0:
            return x;
        case 1:
            return y;
        default:
            throw std::out_of_range("Vec2 index out of range.");
        }
    }
};