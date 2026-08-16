#include "Eclipsea/Gameplay/Sprite/Mob.h"
#include "OpenCore.hpp"
#include <memory>

Mob::Mob(std::string_view textureName, uint8_t gridCols, uint8_t gridRows)
{
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

void Mob::Draw()
{
    if (texture)
    {
        auto info = OpenEngine::getInstance().getGameInfo();

        Rect dstRect = getLogicalBounds();
        Rect shadowRect = {dstRect.x, dstRect.y + 0.5f * (dstRect.h), dstRect.w,
                           dstRect.h * 0.5f};
        Rect windowRect{0, 0, static_cast<float>(info->_graphicsInfo.resolutionWidth),
                        static_cast<float>(info->_graphicsInfo.resolutionHeight)};

        if (visible(dstRect, windowRect) && VState->getAlpha() > 0.0f)
        {
            auto GFX = OpenCoreManagers::GFXManager;

            // 渲染影子
            if (!shadow)
            {
                auto *package = OpenEngine::getInstance().getPackageManager();
                shadow = make_shared<Texture>(
                    1, 1, package ? package->getTextureAsync("entity_shadow")
                                  : nullptr);
            }

            GFX.Draw(shadow->get(), nullptr, &shadowRect, 0.0f, nullptr);

            // 渲染实体

            Rect srcRect =
                (tileWidth > 1 || tileHeight > 1)
                    ? texture->getSubRect(VState->getFrameIndex(), tileWidth, tileHeight)
                    : Rect{texture->getSubRect(VState->getFrameIndex())};
            GFX.Draw(texture->get(), &srcRect, &dstRect, 0.0f, nullptr);
        }
    }
}