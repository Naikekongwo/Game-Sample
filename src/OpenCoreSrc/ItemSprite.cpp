#include "OpenCore/Runtime/Graphics/Sprite/ItemSprite.hpp"
#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"
#include <memory>

ItemSprite::ItemSprite()
{
    id = "null";
    this->layer = 0;
    this->VState = std::make_unique<VisualState>();
    this->AnimeManager = std::make_unique<AnimationManager>();

    texture.reset();
    onEnter();
}

void ItemSprite::onEnter()
{
    texture = std::make_unique<Texture>(
        1, 2, OpenCoreManagers::ResManager.GetTexture(itemTexID));
}

void ItemSprite::Draw()
{
    // <TODO>

    auto GFX = OpenCoreManagers::GFXManager.getInstance();

    Rect rect = getLogicalBounds();
    Rect srcRect = texture->getSubRect(0);

    GFX.Draw(texture->get(), &srcRect, &rect, 0.0f, nullptr);
}