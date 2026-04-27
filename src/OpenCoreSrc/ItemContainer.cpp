#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Animation/IAnimation.hpp"
#include <cstdint>

ItemContainer::ItemContainer(const string &id, uint8_t layer,
                             unique_ptr<Texture> texture, short col, short row)
    : UIElement(id, layer, std::move(texture))
{
    // 初始化

    if (!texture)
    {
        LOG("物品栏初始化时遇到错误，其纹理不应当是空的！ ID {}", id);
        return;
    }

    if (col * row == 0)
    {
        LOG("物品栏初始化失败，不允许容量为0");
        return;
    }

    columns = col;
    rows = row;

    LOG("物品栏初始化成功, ID {}", id);
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
    // 自绘制逻辑
    if (VState->getAlpha() <= 0.0f)
    {
        return;
    }

    if (!texture)
    {
        return;
    }

    auto GFX = OpenCoreManagers::GFXManager.getInstance();

    Rect bounds = getLogicalBounds();

    float offsetX = 0.0f;
    float offsetY = 0.0f;

    float width = bounds.w / columns;
    float height = bounds.h / rows;

    LOG("{} {} {} {}", offsetX, offsetY, width, height);

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {

            // 第i行，第j列
            offsetX = width * j;
            offsetY = height * i;

            Rect dstRect = {bounds.x + offsetX, bounds.y + offsetY, width,
                            height};

            GFX.Draw(texture->get(), nullptr, &dstRect, 0.0f, nullptr);
        }
    }

    m_item->Draw();
}

void ItemContainer::setSize(short row, short col)
{
    if (row * col == 0)
    {
        LOG("输入的行列容量为空！");
        return;
    }

    // 计算出当前的单位高度

    float heightFactor = static_cast<float>(absHeight) / row;

    rows = row;
    columns = col;

    absHeight = heightFactor * row;
    absWidth = col * heightFactor;
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
}