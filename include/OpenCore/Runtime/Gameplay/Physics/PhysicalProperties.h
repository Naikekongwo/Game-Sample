#pragma once

// PhysicalProperties
// 生物的物理属性

#include "OpenCore/Core/Math/OpenCore_Vec2.hpp"
#include "OpenCore/Core/Math/OpenCore_Vec3.hpp"

using Vec3 = OpenCore_Vec3;
using Vec2 = OpenCore_Vec2;

class PhysicalProperties
{
  public:
  private:
    //   玩家的位置和速度属性
    Vec2 Speed{0, 0};
    Vec2 Position{0, 0};
};