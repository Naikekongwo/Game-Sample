#include "OpenCore/Runtime/Graphics/Sprite/HealthBar.hpp"
#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"

void HealthBar::onEnter()
{
    texture = make_unique<Texture>(
        1, 1, OpenCoreManagers::ResManager.GetTexture(texID));

    backBuffer = make_unique<Texture>(
        1, 1, OpenCoreManagers::ResManager.GetTexture(backID));
}

void HealthBar::Draw()
{
    if (VState->getAlpha() <= 0.0f)
        return;

    if (!texture | !backBuffer)
    {
        LOG("空纹理");
        return;
    }

    Rect dstRect = getLogicalBounds();

    auto GFX = OpenCoreManagers::GFXManager.getInstance();

    GFX.Draw(texture->get(), nullptr, &dstRect, 0.0f, nullptr);
}

HealthBar::HealthBar()
{
    this->id = "HealthBar";
    this->layer = 0;

    this->VState = std::make_unique<VisualState>();
    this->AnimeManager = std::make_unique<AnimationManager>();
}