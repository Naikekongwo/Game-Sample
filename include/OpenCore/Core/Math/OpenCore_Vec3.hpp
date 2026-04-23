/**
 * @file OpenCore_Vec3.hpp
 * @author your name (you@domain.com)
 * @brief OpenCore引擎定义的三维向量计算类
 * @version 0.1
 * @date 2026-04-23
 *
 * @copyright Copyright (c) 2026
 *
 */

#ifndef _OPENCORE_VEC3_HPP_
#define _OPENCORE_VEC3_HPP_

#include <cmath>
#include <stdexcept>
#include <vector>

using std::vector;

/**
 * @brief OpenCore引擎三维向量类
 *
 */
class OpenCore_Vec3
{
  public:
    float x, y, z;

    OpenCore_Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    OpenCore_Vec3(float val) : x(val), y(val), z(val) {}
    OpenCore_Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    // 加减
    OpenCore_Vec3 operator+(const OpenCore_Vec3 &other)
    {
        return OpenCore_Vec3(x + other.x, y + other.y, z + other.z);
    }

    OpenCore_Vec3 operator-(const OpenCore_Vec3 &other)
    {
        return OpenCore_Vec3(x - other.x, y - other.y, z - other.z);
    }

    // 等于
    OpenCore_Vec3 operator==(const OpenCore_Vec3 &other)
    {
        return (x == other.x and y == other.y and z == other.z);
    }

    // 点乘（Dot Product） - 返回标量
    float dot(const OpenCore_Vec3 &other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    // 叉乘（Cross Product） - 返回向量
    OpenCore_Vec3 cross(const OpenCore_Vec3 &other) const
    {
        return OpenCore_Vec3(y * other.z - z * other.y, // i分量
                             z * other.x - x * other.z, // j分量
                             x * other.y - y * other.x  // k分量
        );
    }

    // 返回单位向量
    OpenCore_Vec3 normalized()
    {
        float len = sqrt(x * x + y * y + z * z);
        return OpenCore_Vec3(x / len, y / len, z / len);
    }

    // 将自身单位化
    OpenCore_Vec3 &normalize()
    {
        float len = sqrt(x * x + y * y + z * z);
        if (len != 0)
        {
            x /= len;
            y /= len;
            z /= len;
        }
        return *this;
    }

    // 索引访问
    float &operator[](int index)
    {
        switch (index)
        {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        default:
            throw std::out_of_range("Vec3 index of range.");
        }
    }
};

using Vec3 = OpenCore_Vec3;

#endif //_OPENCORE_VEC3_HPP_