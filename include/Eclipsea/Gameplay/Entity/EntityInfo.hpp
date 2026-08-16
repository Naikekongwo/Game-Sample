#pragma once

// EntityInfo.hpp
// 实体的注册类

#include <cstdint>
#include <vector>

#include "Eclipsea/Gameplay/TextureMeta.hpp"

/// @brief 图集中某一格的完整引用：图集元数据 + 格坐�?
struct TextureCell
{
    EclipseaTextureMeta meta;       // 图集描述 (textureID, cols, rows)
    uint8_t originCol = 0;  // 该格在精灵表中的�?
    uint8_t originRow = 0;  // 该格在精灵表中的�?
};

/// @brief 动画组：一个方向的移动动画
struct AnimationGroup
{
    short id = 0;                       // 1=�?2=�?3=�?4=�? >4 扩展
    uint8_t frameRate = 8;              // 每秒帧数
    std::vector<TextureCell> frames;   // 每帧对应的纹理格�?
};

struct EntityInfo
{
    short EntityTypeID = 0;
    TextureCell texture;   // 静止时的默认贴图

    std::vector<AnimationGroup> animations; // 移动动画�?

    uint8_t backpackCapacity = 0;
    uint8_t movable = 1; // 1=可移�? 0=不可移动

    // widthFactor/heightFactor 的因数是一个瓦片的�?�?
    // 代表这个实体占几�?
    float widthFactor = 1.0f;
    float heightFactor = 1.0f;

    /// @brief 快速构造角色（自动生成4方向动画，标�?x4精灵表排布）
    /// 精灵表布局约定：行0=�? �?=�? �?=�? �?=�?
    static EntityInfo makeCharacter(short typeID, short texID,
                                    uint8_t capacity = 0, bool canMove = true,
                                    float wFactor = 1.0f, float hFactor = 1.0f)
    {
        EntityInfo info{};
        info.EntityTypeID = typeID;
        info.texture = {{texID, 4, 4}, 0, 0};
        info.backpackCapacity = capacity;
        info.movable = canMove ? 1 : 0;
        info.widthFactor = wFactor;
        info.heightFactor = hFactor;
        if (canMove)
        {
            info.animations = {
                {1, 8, { {{texID, 4, 4}, 0, 3}, {{texID, 4, 4}, 1, 3}, {{texID, 4, 4}, 2, 3}, {{texID, 4, 4}, 3, 3} }},
                {2, 8, { {{texID, 4, 4}, 0, 0}, {{texID, 4, 4}, 1, 0}, {{texID, 4, 4}, 2, 0}, {{texID, 4, 4}, 3, 0} }},
                {3, 8, { {{texID, 4, 4}, 0, 1}, {{texID, 4, 4}, 1, 1}, {{texID, 4, 4}, 2, 1}, {{texID, 4, 4}, 3, 1} }},
                {4, 8, { {{texID, 4, 4}, 0, 2}, {{texID, 4, 4}, 1, 2}, {{texID, 4, 4}, 2, 2}, {{texID, 4, 4}, 3, 2} }}
            };
        }
        return info;
    }
};
