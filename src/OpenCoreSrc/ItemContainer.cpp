#include "OpenCore/Runtime/Graphics/UI/ItemContainer.hpp"
#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Animation/IAnimation.hpp"
#include "OpenCore/Runtime/Gameplay/Backpack/ItemManager.hpp"
#include <cstdint>
#include <stdexcept>

ItemContainer::ItemContainer(string_view id, uint8_t layer,
                             unique_ptr<Texture> texture, short col, short row)
    : UIElement(id.data(), layer, std::move(texture))
{
    if (col * row == 0)
    {
        LOG("物品栏初始化失败，不允许容量为0");
        return;
    }

    m_columns = col;

    LOG("物品栏初始化成功, ID {}, 指定列数 {}", id, m_columns);
}

void ItemContainer::setBackpack(shared_ptr<Backpack> backpack)
{
    if (!backpack)
    {
        LOG("绑定了一个空背包！ ID:{}", id);
        return;
    }

    // 设置背包引用
    m_backpack = backpack;
}

void ItemContainer::Draw()
{
    if (VState->getAlpha() <= 0.0f)
    {
        return;
    }

    if (!texture)
    {
        return;
    }

    auto backpack = m_backpack.lock();
    if (!backpack)
    {
        LOG("所绑定的背包已经悬空，无法绘制 {}", id);
        return;
    }

    // 以上是检查绘制状态，避免进行不必要的绘制

    auto GFX = OpenCoreManagers::GFXManager.getInstance();

    Rect bounds = getLogicalBounds();

    float offsetX = 0.0f;
    float offsetY = 0.0f;

    short rows = backpack->getCapacity() / m_columns;

    float width = bounds.w / m_columns;
    float height = bounds.h / rows;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < m_columns; j++)
        {

            // 第i行，第j列
            offsetX = width * j;
            offsetY = height * i;

            Rect dstRect = {bounds.x + offsetX, bounds.y + offsetY, width,
                            height};

            GFX.Draw(texture->get(), nullptr, &dstRect, 0.0f, nullptr);
        }
    }

    // throw std::runtime_error("OK");

    // m_item->Draw();
}

void ItemContainer::onEnter()
{
    // 初始化
    m_item = std::make_unique<ItemSprite>();

    m_item->Configure()
        .Parent(this)
        .Anchor(AnchorPoint::Center)
        .Alpha(1.0f)
        .Scale(0.0f, 0.9f)
        .Posite(0.5f, 0.5f);

    auto backpack = m_backpack.lock();
    if (!backpack)
    {
        LOG("背包已经悬空, id {}", id);
        return;
    }

    if (parentContainer)
    {
        LOG("有父组件");
        Rect bounds = parentContainer->getLogicalBounds();
        Rect self_bounds = getLogicalBounds();

        LOG("{} {} {} {}", bounds.x, bounds.y, bounds.w, bounds.h);
        LOG("{}", absWidth);

        // <根据父容器来Resize>
        float newWidth = bounds.w * (self_bounds.w / bounds.w);
        absHeight = (newWidth / m_columns) *
                    (backpack->getCapacity() / static_cast<float>(m_columns));
        absWidth = newWidth;
    }
}

void ItemContainer::setIndexRange(pair<uint8_t, uint8_t> indexRange)
{
    // 设置物品栏的对应索引
    auto backpack = m_backpack.lock();
    if (!backpack)
    {
        LOG("引用的背包已经悬空，ID {}", id);
        return;
    }

    if (indexRange.second >= backpack->getCapacity())
    {
        // Index out of range
        // 超出了背包的容量
        LOG("输入的index索引范围超出了背包的极限, ID{}", id);
        return;
    }

    this->m_indexRange = indexRange;
}