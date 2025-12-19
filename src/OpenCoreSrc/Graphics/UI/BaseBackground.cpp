#include "OpenCore/OpenCore.hpp"
#include "Eclipsea/Eclipsea.hpp"

BaseBackground::BaseBackground(const std::string& id, uint8_t layer, std::unique_ptr<Texture> texture)
{

    this->id = id;
    this->layer = layer;

    // 获取材质
    if (!texture)
    {
        SDL_Log("BaseBackground::BaseBackground() encountered a empty texture.");
        return;
        // 如果材质为空，那么我们直接强制返回
    }

    this->texture = std::move(texture);
}

void BaseBackground::setNativeScale(uint8_t scale)
{
    nativeScale = scale;
}

void BaseBackground::handlEvents(SDL_Event &event, float totalTime)
{
    if(event.type == SDL_WINDOWEVENT)
    {
        if(event.window.event == SDL_WINDOWEVENT_RESIZED ||
           event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
        {
            if(directRender)
            {
                setBakedTexture(true);
            }
        }
    }
}

int BaseBackground::setBakedTexture(bool isBaked)
{
    directRender = isBaked;

    // OpenCore_Rect Borders = getBounds();

    // TextureBuffer.reset(GraphicsManager::getInstance().createTexture(Borders.w, Borders.h));
    // preRenderTexture(TextureBuffer.get());
    return 0;
}

bool BaseBackground::preRenderTexture(OpenCore_Tilemap *target)
{
    auto &GFX = GraphicsManager::getInstance();
    if (!target) return false;

    GFX.setOffScreenRender(target);

    int texW, texH;
    texture->texture.get()->QueryTile(texW, texH);

    OpenCore_Rect srcRect{};
    OpenCore_Rect dstRect{};

    // 离屏纹理宽高
    int targetW, targetH;
    target->QueryTile(targetW, targetH);

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

            GFX.RenderTile(*texture->texture.get(), &srcRect, &dstRect, {0.0, nullptr, 0});
        }
    }

    GFX.setOffScreenRender(nullptr);
    return true;
}
