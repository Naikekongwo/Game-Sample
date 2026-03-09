
#include "OpenCore/OpenCore.hpp"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <cstddef>
#include <memory>

BaseBackground::BaseBackground(const std::string &id, uint8_t layer,
                               unique_ptr<Texture> texture)
    : UIElement(id, layer, std::move(texture))
{

    this->id = id;
    this->layer = layer;

    // 获取材质
    if (!texture)
    {
        Console_Log(
            "BaseBackground::BaseBackground() encountered a empty texture.");
        return;
        // 如果材质为空，那么我们直接强制返回
    }
    this->texture = std::move(texture);
}

void BaseBackground::onUpdate(float totalTime)
{
    if (status != BaseBackgroundStatus::ready)
    {
        onEnter();
    }
}
void BaseBackground::onEnter()
{
    auto &GFX = OpenCoreManagers::GFXManager;
    SDL_Rect bounds = getLogicalBounds();
    TextureCache = GFX.createTexture(bounds.w, bounds.h);
    generateTexture(TextureCache);
}

void BaseBackground::onExit()
{
    if (TextureCache)
    {
        SDL_DestroyTexture(TextureCache);
        TextureCache = nullptr;
    }
}

void BaseBackground::Draw()
{
    auto &GFX = OpenCoreManagers::GFXManager.getInstance();

    Rect logiRect = getLogicalBounds();
    Rect VRect = GFX.getSccissorRect();

    if (TextureCache && visible(logiRect, VRect) && VState->getAlpha() > 0.0f)
    {
        Rect dstRect = getPhysicalBounds();

        SDL_SetTextureAlphaMod(TextureCache, VState->getAlpha());
        GFX.Draw(TextureCache, NULL, &dstRect, 0, 0);
    }
}

void BaseBackground::setNativeScale(uint8_t scale) { nativeScale = scale; }

void BaseBackground::handlEvents(SDL_Event &event, float totalTime)
{
    switch (event.type)
    {
    case SDL_WINDOWEVENT:
    {
        if (event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            auto &GFX = OpenCoreManagers::GFXManager;
            SDL_Rect bounds = getLogicalBounds();
            if (TextureCache)
            {
                SDL_DestroyTexture(TextureCache);
            }
            TextureCache = GFX.createTexture(bounds.w, bounds.h);
            generateTexture(TextureCache);
        }
    }
    default:
        break;
    }
}

bool BaseBackground::generateTexture(SDL_Texture *target)
{
    auto &GFX = GraphicsManager::getInstance();
    if (!target)
        return false;

    GFX.setOffScreenRender(target);

    int texW, texH;
    SDL_QueryTexture(texture->texture.get(), nullptr, nullptr, &texW, &texH);

    SDL_Rect srcRect{};
    SDL_Rect dstRect{};

    // 离屏纹理宽高
    int targetW, targetH;
    SDL_QueryTexture(target, nullptr, nullptr, &targetW, &targetH);

    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
        {
            srcRect = texture->getSubRect(row * 3 + col);

            // X方向
            if (col == 0)
            {
                dstRect.x = 0;
                dstRect.w = nativeScale;
            }
            else if (col == 1)
            {
                dstRect.x = nativeScale;
                dstRect.w = targetW - 2 * nativeScale;
            }
            else
            {
                dstRect.x = targetW - nativeScale;
                dstRect.w = nativeScale;
            }

            // Y方向
            if (row == 0)
            {
                dstRect.y = 0;
                dstRect.h = nativeScale;
            }
            else if (row == 1)
            {
                dstRect.y = nativeScale;
                dstRect.h = targetH - 2 * nativeScale;
            }
            else
            {
                dstRect.y = targetH - nativeScale;
                dstRect.h = nativeScale;
            }

            GFX.RenderCopyEx(texture->texture.get(), &srcRect, &dstRect, 0.0,
                             nullptr, SDL_FLIP_NONE);
        }
    }

    GFX.setOffScreenRender(nullptr);

    status = BaseBackgroundStatus::ready;
    return true;
}
