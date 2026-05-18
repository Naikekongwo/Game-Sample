#pragma once

#include "OpenCore/Core/Math/OpenCore_Vec3.hpp"
#include "OpenCore/Runtime/Gameplay/Backpack/Iteminfo.hpp"
#include <memory>
#include <string>

using std::string;
using std::unique_ptr;

class ItemSprite; // 前向声明
class Entity;     // 前向声明（用于 owner 指针）

enum class ItemStatus
{
    Registered, // 已注册
    Ready       // 完全就绪
};

struct ItemTextureMeta
{
    short textureID;
    uint8_t texture_rows;
    uint8_t texture_cols;
};

class Item
{
  public:
    Item() = default;
    Item(const ItemInfo &itemInfo);

    const string &getID() const noexcept { return id; }
    short getTypeID() const noexcept { return info.typeID; }
    bool isReady() const noexcept { return status == ItemStatus::Ready; }

    ItemInfo getItemInfo() { return info; }

  private:
    string id; // 物品实例的唯一 ID

    ItemInfo info;
    ItemStatus status = ItemStatus::Registered;

    float widthRelative = 1.0f;  // 屏幕宽度缩放因子
    float heightRelative = 1.0f; // 屏幕高度缩放因子
};