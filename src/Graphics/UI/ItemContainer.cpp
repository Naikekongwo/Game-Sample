#include "OpenCore/OpenCore.hpp"

ItemContainer::ItemContainer(std::string id, short layer, Texture* texture)
{
    this->id = id;
    this->layer = layer;

    // 关闭预烘焙功能
    // 因为我觉得这玩意也不需要预烘焙
    this->directRender = false;


    if (!texture)
    {
        SDL_Log("ItemContainer::ItemContainer() encountered a empty texture when initialize.");
        return;
    }

    this->texture = texture;
}

void ItemContainer::onRender()
{
    auto& GFX = OpenCoreManagers::GFXManager.getInstance();

    SDL_Rect dstRect = getBounds();

    GFX.RenderCopyEx(texture->texture.get(),NULL, &dstRect, AnimeState->angle, NULL, SDL_FLIP_NONE);
}

