// ItemInfo.hpp
#pragma once
#include <string>

class Entity;  // 前向声明，允许使用 Entity* 指针

struct ItemInfo
{
    std::string id;          // 物品实例的唯一 ID
    unsigned int typeID = 0; // 物品类型
    short textureID = 0;       // 纹理 ID（用于 ItemSprite）
    float widthFactor = 1.0f;// 缩放因子
    unsigned int statueID = 0; // 物品状态 ID（可用于不同状态显示不同纹理）
    Entity* owner = nullptr; // 物品所属实体（如果有）
    // 其他可能需要的字段...
};