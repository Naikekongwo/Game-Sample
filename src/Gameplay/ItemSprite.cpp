#include "Eclipsea/Gameplay/Sprite/ItemSprite.hpp"
#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore.hpp"
#include <memory>

ItemSprite::ItemSprite() {
  id = "null";
  this->layer = 0;
  this->VState = std::make_unique<VisualState>();
  this->AnimeManager = std::make_unique<AnimationManager>();

  texture.reset();
  onEnter();
}

ItemSprite::ItemSprite(std::string_view textureName) {
  this->id = "null";
  this->layer = 0; // 适当层级

  this->VState = std::make_unique<VisualState>();
  this->AnimeManager = std::make_unique<AnimationManager>();

  auto *package = OpenEngine::getInstance().getPackageManager();
  this->texture = std::make_unique<Texture>(
      4, 4, package ? package->getTextureAsync(textureName) : nullptr);

  LOG("物品精灵创建成功，纹理:{}", textureName);
}

void ItemSprite::onEnter() {
  auto *package = OpenEngine::getInstance().getPackageManager();
  texture = std::make_unique<Texture>(
      1, 2, package ? package->getTextureAsync(itemTexName) : nullptr);
}

void ItemSprite::Draw() {
  // <TODO>

  Rect rect = getLogicalBounds();
  Rect srcRect = texture->getSubRect(VState->getFrameIndex());

  texture->Draw(&srcRect, &rect, 0.0, nullptr);
}

bool ItemSprite::setSubTexture(short index) {
  if (!texture)
    return false;

  if (index >= texture->Size())
    return false;

  VState->frameIndex = index;
  return true;
}