#include "Eclipsea/Gameplay/Sprite/Mob.h"
#include "OpenCore.hpp"
#include <memory>

Mob::Mob(std::string_view textureName, uint8_t gridCols, uint8_t gridRows) {
  this->id = "MobSprite";
  this->layer = 0;

  this->VState = std::make_unique<VisualState>();
  this->AnimeManager = std::make_unique<AnimationManager>();

  auto *package = OpenEngine::getInstance().getPackageManager();
  this->texture = std::make_unique<Texture>(
      gridCols, gridRows,
      package ? package->getTextureAsync(textureName) : nullptr);

  LOG("生物创建成功，纹理:{}, 网格:{}x{}", textureName, gridCols, gridRows);
}

void Mob::Draw() {
  if (texture) {
    auto info = OpenEngine::getInstance().getGameInfo();

    Rect dstRect = getLogicalBounds();
    Rect shadowRect = {dstRect.x, dstRect.y + 0.5f * (dstRect.h), dstRect.w,
                       dstRect.h * 0.5f};
    Rect windowRect{0, 0,
                    static_cast<float>(info->_graphicsInfo.resolutionWidth),
                    static_cast<float>(info->_graphicsInfo.resolutionHeight)};

    if (visible(dstRect, windowRect) && VState->getAlpha() > 0.0f) {
      // 渲染影子
      if (!shadow) {
        auto *package = OpenEngine::getInstance().getPackageManager();
        shadow = make_shared<Texture>(
            1, 1,
            package ? package->getTextureAsync("entity_shadow") : nullptr);
      }

      shadow->Draw(nullptr, &shadowRect, 0.0, nullptr);

      // 渲染实体
      Rect srcRect;
      if (tileWidth > 1 || tileHeight > 1) {
        // 多瓦片精灵：以当前帧为左上角，截取 tileWidth×tileHeight 瓦片的子矩形
        const size_t frame = static_cast<size_t>(VState->getFrameIndex());
        const uint16_t frameW = texture->width;
        const uint16_t frameH = texture->height;
        const size_t col = frame % texture->xCount;
        const size_t row = frame / texture->xCount;
        srcRect = Rect{static_cast<float>(col * frameW),
                       static_cast<float>(row * frameH),
                       static_cast<float>(frameW * tileWidth),
                       static_cast<float>(frameH * tileHeight)};
      } else {
        srcRect = texture->getSubRect(VState->getFrameIndex());
      }
      texture->Draw(&srcRect, &dstRect, 0.0, nullptr);
    }
  }
}