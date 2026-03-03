
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Animation/AnimationPipeline.hpp"

ImageBoard::ImageBoard(const std::string &id, uint8_t layer,
                       unique_ptr<Texture> texture)
    : UIElement(id, layer, std::move(texture))
{
    // 设置ID 层级属性
    this->id = id;
    this->layer = layer;

    // 获取材质
    if (!texture)
    {
        Console_Log("ImageBoard::ImageBoard() encountered a empty texture.");
        return;
        // 如果材质为空，那么我们直接强制返回
    }

    this->texture = std::move(texture);
}

void ImageBoard::onRender()
{
    auto &GFX = GraphicsManager::getInstance();
    // 渲染函数
    if (texture->get())
    {
        SDL_SetTextureAlphaMod(texture->texture.get(), AnimeState->getAlpha());

        Rect dstRect = getLogicalBounds();
        Rect srcRect = texture->getSubRect(AnimeState->getFrameIndex());

        dstRect = magnetRect(dstRect);

        GFX.Draw(texture->texture.get(), &srcRect, &dstRect,
                 AnimeState->getAngle(), NULL);
    }
}
