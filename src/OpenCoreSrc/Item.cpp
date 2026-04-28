#include "OpenCore/Runtime/Gameplay/Backpack/Item.hpp"
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Graphics/Sprite/ItemSprite.hpp"
#include "OpenCore/Runtime/Gameplay/Entity/Entity.hpp"
#include "OpenCore/Core/Math/OpenCore_Vec3.hpp"
#include <cmath>

Item::~Item()= default;
void Item::Configure(const ItemInfo &itemInfo)
{
    info = itemInfo;
    typeID = info.typeID;
    id = info.id;
    statueID = info.statueID;
    owner = info.owner;            // 从配置中获取归属实体
    LOG("物品配置成功，类型ID：{}，状态ID：{}", info.typeID, info.statueID);
    status = ItemStatus::Ready;
}

void Item::createRenderer()
{
    if (!renderer && info.textureID != 0)
    {
        renderer = std::make_unique<ItemSprite>(static_cast<short>(info.textureID));
        renderer->Configure().Anchor(AnchorPoint::BottomCenter).Alpha(0.0f);

        widthRelative = 1.0f / *OpenCoreManagers::SetManager.getRenderWidth();
        heightRelative = 1.0f / *OpenCoreManagers::SetManager.getRenderHeight();

        renderer->Configure().Scale(info.widthFactor * widthRelative, 0.0f);
    }
}

OpenCore_Vec3 Item::GetPosition() const
{
    if (owner)
    {
        return owner->getPhysicalProperties().getPosition();
    }
    // 无归属实体时返回原点，视口裁剪会将其排除
    return OpenCore_Vec3(0.0f, 0.0f, 0.0f);
}

void Item::Draw(float cameraX, float cameraY)
{
    if (!drawable)
    {
        if (renderer)
            renderer.reset();
        return;
    }

    if (!owner){
        LOG("物品未绑定实体，无法绘制");
        return;
    }
    if (!renderer)
        createRenderer();

    Vec3 position = GetPosition();

    auto renderWidth = *OpenCoreManagers::SetManager.getRenderWidth();
    auto renderHeight = *OpenCoreManagers::SetManager.getRenderHeight();
    renderWidth = (renderWidth - 2) / 2 + 1;
    renderHeight = (renderHeight - 1) / 2 + 1;

    // 视口裁剪
    if ((std::abs(cameraX - position.x) > renderWidth) ||
        (cameraY - position.y > renderHeight))
    {
        return;
    }

    renderer->setPosition(0.5f + (position.x - cameraX) * widthRelative,
                          0.5f + (position.y - cameraY) * heightRelative);
    renderer->setTransparency(1.0f);

    // 可根据 statueID 切换纹理帧，例如：
    // renderer->getVisualState()->frameIndex = statueID;

    renderer->Draw();
}

void Item::onUpdate(float totalTime)
{
    // 物品暂无可更新的逻辑
}

void Item::enableDrawer(bool enabled)
{
    drawable = enabled;
    status = enabled ? ItemStatus::Drawable : ItemStatus::Registered;
}

void Item::Spawn(Entity* newOwner)
{
    owner = newOwner;   // 绑定归属实体（如掉落时重新绑定）
}