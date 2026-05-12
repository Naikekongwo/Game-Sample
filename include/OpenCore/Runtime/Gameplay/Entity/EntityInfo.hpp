#pragma once

// EntityInfo.hpp
// 实体的注册类

#include <cstdint>

struct EntityInfo
{
    short EntityTypeID = 0;
    short TextureID = 0;

    uint8_t backpackCapacity = 0;

    // widthFactor/heightFactor 的因数是一个瓦片的宽/高
    // 代表这个实体占几格
    float widthFactor = 1.0f;
    float heightFactor = 1.0f;

    uint8_t frameCount = 4; // 每个方向的动画帧数（4×4精灵表默认4帧）
};