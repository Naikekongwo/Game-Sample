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

ItemSprite::ItemSprite(short textureID)
{
    this->id = "null";
    this->layer = 0; // 适当层级

    this->VState = std::make_unique<VisualState>();
    this->AnimeManager = std::make_unique<AnimationManager>();

    this->texture = std::make_unique<Texture>(
        4, 4, OpenCoreManagers::ResManager.GetTexture(textureID));

    LOG("物品精灵创建成功，纹理ID:{}", textureID);
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
    Rect srcRect = texture->getSubRect(VState->getFrameIndex());

    GFX.Draw(texture->get(), &srcRect, &rect, 0.0f, nullptr);
}

bool ItemSprite::setSubTexture(short index)
{
    if (!texture)
        return false;

    if (index >= texture->Size())
        return false;

    VState->frameIndex = index;
    return true;
}