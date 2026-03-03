#pragma once

// OpenCore_Collision
// 碰撞箱类
#include "OpenCore/Core/Math/OpenCore_Rect.hpp"

struct Collision_Box
{
    Rect Box;

    Collision_Box() : Box{0, 0, 0, 0} {};
    Collision_Box(const Rect &rect) : Box{rect.x, rect.y, rect.w, rect.h} {};

    // 转化为OpenCore_Rect
    operator Rect() const { return Box; }
};