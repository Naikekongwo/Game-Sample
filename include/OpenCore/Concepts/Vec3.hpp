#ifndef _VEC3_HPP_
#define _VEC3_HPP_

// Vec3
// 三维矢量

#include <algorithm>

class Vec3
{
public:
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float val) : x(val), y(val), z(val) {}
    Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    
    // 加法
    Vec3 operator+(const Vec3 &other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    // 减法
    Vec3 operator-(const Vec3 &other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }
    
    // 乘法（向量点乘）
    Vec3 operator*(const Vec3 &other) const {
        return Vec3(x * other.x, y * other.y, z * other.z);
    }
    
    // 除法（向量分量除）
    Vec3 operator/(const Vec3 &other) const {
        return Vec3(x / other.x, y / other.y, z / other.z);
    }
    
    // 标量乘法
    Vec3 operator*(float scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }
    
    // 标量除法
    Vec3 operator/(float scalar) const {
        float inv = 1.0f / scalar;
        return Vec3(x * inv, y * inv, z * inv);
    }
    
    // 友元标量乘法（支持 scalar * Vec3）
    friend Vec3 operator*(float scalar, const Vec3& vec) {
        return vec * scalar;
    }
    
    // 复合赋值运算
    Vec3& operator+=(const Vec3 &other) {
        x += other.x; y += other.y; z += other.z;
        return *this;
    }
    
    Vec3& operator-=(const Vec3 &other) {
        x -= other.x; y -= other.y; z -= other.z;
        return *this;
    }
    
    Vec3& operator*=(float scalar) {
        x *= scalar; y *= scalar; z *= scalar;
        return *this;
    }
    
    Vec3& operator/=(float scalar) {
        float inv = 1.0f / scalar;
        x *= inv; y *= inv; z *= inv;
        return *this;
    }
    
    // 一元负号
    Vec3 operator-() const {
        return Vec3(-x, -y, -z);
    }
    
    // 比较运算
    bool operator==(const Vec3 &other) const {
        const float epsilon = 0.0001f;
        return std::abs(x - other.x) < epsilon &&
               std::abs(y - other.y) < epsilon &&
               std::abs(z - other.z) < epsilon;
    }
    
    bool operator!=(const Vec3 &other) const {
        return !(*this == other);
    }
    
    // 索引访问
    float& operator[](int index) {
        if (index == 0) return x;
        if (index == 1) return y;
        if (index == 2) return z;
        throw std::out_of_range("Vec3 index out of range");
    }
    
    const float& operator[](int index) const {
        if (index == 0) return x;
        if (index == 1) return y;
        if (index == 2) return z;
        throw std::out_of_range("Vec3 index out of range");
    }
    
    // 点积
    float dot(const Vec3 &other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    
    // 叉积
    Vec3 cross(const Vec3 &other) const {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
    
    // 长度
    float length() const {
        return std::sqrt(x*x + y*y + z*z);
    }
    
    // 长度平方（避免开方，用于比较）
    float lengthSquared() const {
        return x*x + y*y + z*z;
    }
    
    // 归一化（返回新向量）
    Vec3 normalized() const {
        float len = length();
        if (len > 0) {
            return Vec3(x/len, y/len, z/len);
        }
        return *this;
    }
    
    // 归一化（修改自身）
    Vec3& normalize() {
        float len = length();
        if (len > 0) {
            x /= len; y /= len; z /= len;
        }
        return *this;
    }
    
    // 距离
    float distance(const Vec3 &other) const {
        return (*this - other).length();
    }
    
    // 距离平方
    float distanceSquared(const Vec3 &other) const {
        return (*this - other).lengthSquared();
    }
    
    // 限制长度
    Vec3 clampLength(float maxLength) const {
        float len = length();
        if (len > maxLength) {
            return normalized() * maxLength;
        }
        return *this;
    }
    
    // 线性插值
    static Vec3 lerp(const Vec3 &a, const Vec3 &b, float t) {
        t = std::clamp(t, 0.0f, 1.0f);
        return a + (b - a) * t;
    }
    
    // 反射
    Vec3 reflect(const Vec3 &normal) const {
        return *this - normal * (2.0f * this->dot(normal));
    }
    
    // 投影到另一个向量
    Vec3 project(const Vec3 &other) const {
        float lenSq = other.lengthSquared();
        if (lenSq > 0) {
            return other * (this->dot(other) / lenSq);
        }
        return Vec3();
    }
    
    // 是否为零向量
    bool isZero(float tolerance = 0.0001f) const {
        return lengthSquared() < tolerance * tolerance;
    }
    
    // 获取最大值分量
    float maxComponent() const {
        return std::max({x, y, z});
    }
    
    // 获取最小值分量
    float minComponent() const {
        return std::min({x, y, z});
    }
    
    // 常量向量
    static Vec3 zero() { return Vec3(0, 0, 0); }
    static Vec3 one() { return Vec3(1, 1, 1); }
    static Vec3 up() { return Vec3(0, 1, 0); }
    static Vec3 down() { return Vec3(0, -1, 0); }
    static Vec3 right() { return Vec3(1, 0, 0); }
    static Vec3 left() { return Vec3(-1, 0, 0); }
    static Vec3 forward() { return Vec3(0, 0, 1); }
    static Vec3 back() { return Vec3(0, 0, -1); }
    
};

#endif //_VEC3_HPP_