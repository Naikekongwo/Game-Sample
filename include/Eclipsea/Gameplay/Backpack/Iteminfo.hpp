// ItemInfo.hpp
#pragma once
#include <string>

class Entity; // 前向声明，允许使用 Entity* 指针

struct ItemInfo
{
    std::string id;          // 物品实例的唯一 ID
    short typeID = 0;        // 物品类型
    short textureMetaID = 0; // 纹理 ID（用于 ItemSprite）
    short texturePosID = 0;
    float widthFactor = 1.0f;     // 缩放因子
    uint8_t maxStackedAmount = 0; // 最大堆叠数量
};