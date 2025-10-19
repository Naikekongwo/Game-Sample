#include "OpenCore/OpenCore.hpp"


ItemContainerCollection::ItemContainerCollection(std::string id, short layer, short texID, short rows, short cols)
{
    this->id = id;
    this->layer = layer;

    if(OpenCoreManagers::ResManager.GetTexture(texID) == nullptr)
    {
        // 纹理为空
        SDL_Log("ItemContainerCollection::~() encountered a empty texture, what r u doing in programming pal?");
        return;
    }

    if(rows*cols == 0)
    {
        // 容量为空
        SDL_Log("ItemContainerCollection::~() you trying to create a itemContainer with capacity of zero, that is illegal!");
        return;
    }

    this->rows = rows;
    this->cols = cols;

    // 提前节省内存
    ItemGroups.reserve(rows*cols);

}

void ItemContainerCollection::pushItem(Item* item, short slotID)
{
    if(slotID < 0 || slotID >= rows * cols)
    {
        return;
    }
    else
    {
        return;
        // 
    }
}

void ItemContainerCollection::onUpdate(float totalTime)
{
    if (GroupIsReady) return; // 已生成直接返回

    SDL_Rect Borders = getBounds();
    const float spacingRatio = 0.1f; // 格子之间间距占格子大小的比例

    // 计算单格大小（保证行列都不超出容器）
    float cellWidth  = Borders.w / (cols + (cols - 1) * spacingRatio);
    float cellHeight = Borders.h / (rows + (rows - 1) * spacingRatio);
    float CSize = std::min(cellWidth, cellHeight);

    // 计算间距
    float spacingX = spacingRatio * CSize;
    float spacingY = spacingRatio * CSize;

    // 容器中心
    float centerX = Borders.x + Borders.w * 0.5f;
    float centerY = Borders.y + Borders.h * 0.5f;

    // 左上角起点
    float startX = centerX - (cols * CSize + (cols - 1) * spacingX) * 0.5f + CSize * 0.5f;
    float startY = centerY - (rows * CSize + (rows - 1) * spacingY) * 0.5f + CSize * 0.5f;

    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            int idx = r * cols + c;

            // 创建 ItemContainer
            auto ItemC = UI<ItemContainer>(std::to_string(idx), static_cast<short>(layer + 1), texID, 1, 1);
            if (!ItemC) continue;

            float posX = startX + c * (CSize + spacingX);
            float posY = startY + r * (CSize + spacingY);

            ItemC->Configure()
                .Anchor(AnchorPoint::Center)
                .Scale(CSize, CSize)
                .Posite(posX, posY);

            ItemGroups.push_back(std::move(ItemC));
        }
    }

    GroupIsReady = true; // 标记只生成一次
}


void ItemContainerCollection::onRender()
{
    for(auto& entry : ItemGroups)
    {
        entry->onRender();
    }
}