#pragma once

// EntityInfo.hpp
// 实体的注册类

#include <cstdint>
#include <vector>

/// @brief 纹理资源描述，绑定纹理ID与精灵表网格尺寸
struct TextureAsset
{
    short textureID = 0;
    uint8_t gridCols = 4;   // 精灵表总列数
    uint8_t gridRows = 4;   // 精灵表总行数
    uint8_t originCol = 0;  // 该实体纹理在精灵表中的起始列
    uint8_t originRow = 0;  // 该实体纹理在精灵表中的起始行
};

/// @brief 动画组：一个方向的移动动画
struct AnimationGroup
{
    short id = 0;                       // 1=上 2=下 3=左 4=右, >4 扩展
    uint8_t frameRate = 8;              // 每秒帧数
    std::vector<TextureAsset> frames;   // 每帧对应的纹理格子
};

struct EntityInfo
{
    short EntityTypeID = 0;
    TextureAsset texture;   // 静止时的默认贴图

    std::vector<AnimationGroup> animations; // 移动动画组

    uint8_t backpackCapacity = 0;

    // widthFactor/heightFactor 的因数是一个瓦片的宽/高
    // 代表这个实体占几格
    float widthFactor = 1.0f;
    float heightFactor = 1.0f;
};
