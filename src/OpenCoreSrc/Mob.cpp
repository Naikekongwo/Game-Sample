#include "OpenCore/OpenCore.hpp"
#include <memory>

Mob::Mob(short TextureID)
{
    this->id = "MobSprite";
    this->layer = 0;

    this->VState = std::make_unique<VisualState>();
    this->AnimeManager = std::make_unique<AnimationManager>();

    this->texture = std::make_unique<Texture>(
        4, 4, OpenCoreManagers::ResManager.GetTexture(TextureID));
    // 定义了该MOB的纹理

    LOG("生物创建成功，纹理ID:{}", TextureID);
}

void Mob::Draw()
{
    if (texture)
    {
        auto info = OpenEngine::getInstance().getGameInfo();

        Rect dstRect = getLogicalBounds();
        Rect windowRect{0, 0, static_cast<float>(info->TargetResolutionWidth),
                        static_cast<float>(info->TargetResolutionHeight)};

        if (visible(dstRect, windowRect) && VState->getAlpha() > 0.0f)
        {
            auto GFX = OpenCoreManagers::GFXManager.getInstance();
            Rect srcRect = texture->getSubRect(VState->getFrameIndex());
            GFX.Draw(texture->get(), &srcRect, &dstRect, 0.0f, nullptr);
        }
    }
}