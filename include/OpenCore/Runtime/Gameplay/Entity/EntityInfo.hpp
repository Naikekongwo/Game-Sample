#pragma once

// EntityInfo.hpp
// 实体的注册类

struct EntityInfo
{
    short EntityTypeID = 0;
    short TextureID = 0;

    // widthFactor的因数是一格的宽度
    float widthFactor = 1.0f;
};