#pragma once

// EntityInfo.hpp
// 实体的注册类

#include <cstdint>
#include <vector>

/// @brief 动画序列中的一帧
struct AnimFrame
{
    uint8_t col = 0;   // 帧左上角在精灵表网格中的列
    uint8_t row = 0;   // 帧左上角在精灵表网格中的行
    uint8_t tileW = 0; // 本帧宽（格），0=沿用实体默认 tileWidth
    uint8_t tileH = 0; // 本帧高（格），0=沿用实体默认 tileHeight
};

/// @brief 一个完整的动作循环（如 idle / walk / attack）
struct AnimSequence
{
    std::vector<AnimFrame> frames;
};

struct EntityInfo
{
    short EntityTypeID = 0;
    short TextureID = 0;

    uint8_t backpackCapacity = 0;

    // widthFactor/heightFactor 的因数是一个瓦片的宽/高
    // 代表这个实体占几格
    float widthFactor = 1.0f;
    float heightFactor = 1.0f;

    uint8_t frameCount = 4; // 每个方向的动画帧数（公式法回退用）

    uint8_t defaultCol = 0; // 静态占位贴图在精灵表网格中的列（无动画实体使用）
    uint8_t defaultRow = 0; // 静态占位贴图在精灵表网格中的行

    // 显式动画序列 —— 非空时优先使用，覆盖公式计算
    std::vector<AnimSequence> animations;
};