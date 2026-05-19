#include "OpenCore/Runtime/Graphics/UI/Symbol.hpp"
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Animation/IAnimation.hpp"
#include "OpenCore/Runtime/Animation/Manager/AnimationManager.hpp"
#include "OpenCore/Runtime/Graphics/IDrawableObject/IDrawableObject.hpp"
#include "OpenCore/Runtime/Graphics/IDrawableObject/UIElement.hpp"
#include <SDL2/SDL_render.h>

Symbol::Symbol(std::string_view id, short layer, short texMetaID)
    : UIElement(id.data(), layer, nullptr)
{
    auto &TMMGR = OpenCoreManagers::TexMetaManager.getInstance();

    auto texOpt = TMMGR.getTexture(texMetaID);
    if (texOpt == std::nullopt)
    {
        LOG("创建元素 {} 时发生错误，根据指定META ID {}未找到对应的贴图",
            id.data(), texMetaID);
        return;
    }

    this->AnimeManager = std::make_unique<AnimationManager>();
    this->VState = std::make_unique<VisualState>();

    neo_texture = texOpt.value();
}

void Symbol::handlEvents(SDL_Event &event, float totalTime) {}

void Symbol::onUpdate(float totalTime)
{

    AnimeManager->onUpdate(totalTime, *VState);
}

void Symbol::Draw()
{
    if (VState->transparency == 0)
        return;

    if (!neo_texture)
        return;

    auto &GFX = OpenCoreManagers::GFXManager.getInstance();

    // 应用透明度效果
    SDL_SetTextureAlphaMod(neo_texture->get(), VState->getAlpha());

    Rect dstRect = getLogicalBounds();
    Rect srcRect = neo_texture->getSubRect(static_cast<int>(m_symbolType));

    GFX.Draw(neo_texture->get(), &srcRect, &dstRect, VState->angle, nullptr);

    // 撤销透明度效果
    SDL_SetTextureAlphaMod(neo_texture->get(), 255);
}
