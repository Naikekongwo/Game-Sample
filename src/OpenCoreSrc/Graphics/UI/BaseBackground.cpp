#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"
#include <SDL2/SDL_render.h>
#include <cstddef>
#include <memory>

BaseBackground::BaseBackground(const std::string &id, uint8_t layer,
                               std::unique_ptr<Texture> texture)
{

    this->id = id;
    this->layer = layer;

    // 获取材质
    if (!texture)
    {
        SDL_Log(
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

void BaseBackground::onRender()
{
    if (TextureCache)
    {
        SDL_Rect dstRect = getPhysicalBounds();
        SDL_SetTextureAlphaMod(
            TextureCache,
            static_cast<uint8_t>(AnimeState->transparency * 255.0f));
        OpenCoreManagers::GFXManager.RenderCopy(TextureCache, NULL, &dstRect, 0,
                                                0, SDL_FLIP_NONE);
    }
}

void BaseBackground::setNativeScale(uint8_t scale) { nativeScale = scale; }

void BaseBackground::handlEvents(SDL_Event &event, float totalTime) {}

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
            srcRect = texture->getSrcRect(row * 3 + col);

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
