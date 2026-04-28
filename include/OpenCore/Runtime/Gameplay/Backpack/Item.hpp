#pragma once

#include <string>
#include <memory>
#include "OpenCore/Runtime/Gameplay/Backpack/Iteminfo.hpp"
#include "OpenCore/Core/Math/OpenCore_Vec3.hpp"

using std::string;
using std::unique_ptr;

class ItemSprite;          // 前向声明
class Entity;              // 前向声明（用于 owner 指针）

enum class ItemStatus
{
    Registered,            // 已注册
    Drawable,              // 可绘制
    Ready                  // 完全就绪
};

class Item
{
public:
    Item() = default;
    virtual ~Item();

    Item(Item&&) = default;
    Item& operator=(Item&&) = default;
    
    const std::string &getID() const noexcept { return id; }
    unsigned int getTypeID() const noexcept { return typeID; }
    unsigned int getStatueID() const noexcept { return statueID; }

    void Spawn(Entity* newOwner = nullptr);   // 绑定归属实体
    void onUpdate(float totalTime);
    void Draw(float cameraX, float cameraY);
    void enableDrawer(bool enabled = true);
    void createRenderer();

    void Configure(const ItemInfo &itemInfo);

    bool Ready() const noexcept { return status >= ItemStatus::Drawable; }

protected:
    // 状态 ID 变化时的钩子，派生类可重写
    virtual void onStatueIDChanged(unsigned int oldID, unsigned int newID) {};

private:
    string id;                      // 物品实例的唯一 ID
    unsigned int typeID = 0;        // 物品类型 ID
    unsigned int statueID = 0;      // 物品状态 ID（控制纹理/表现）
    Entity* owner = nullptr;        // 所属实体（如有）

    unique_ptr<ItemSprite> renderer;
    bool drawable = false;
    ItemInfo info;
    ItemStatus status = ItemStatus::Registered;

    float widthRelative = 1.0f;     // 屏幕宽度缩放因子
    float heightRelative = 1.0f;    // 屏幕高度缩放因子

    // 辅助函数：安全获取世界坐标
    OpenCore_Vec3 GetPosition() const;
};