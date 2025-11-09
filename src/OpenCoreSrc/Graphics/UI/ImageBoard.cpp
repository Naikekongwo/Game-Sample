#include "OpenCore/OpenCore.hpp"
#include "Union.hpp"
#include "OpenCore/Animation/AnimationPipeline.hpp"

ImageBoard::ImageBoard(std::string id, short layer, Texture* texture)
{
    // 设置ID 层级属性
    this->id = id;
    this->layer = layer;

    this->directRender = false;

    // 获取材质
    if (!texture)
    {
        SDL_Log("ImageBoard::ImageBoard() encountered a empty texture.");
        return;
        // 如果材质为空，那么我们直接强制返回
    }

    this->texture = texture;
}


void ImageBoard::onRender()
{
    auto& GFX = GraphicsManager::getInstance();
    // 渲染函数
    SDL_SetTextureAlphaMod(texture->texture.get(), 255.0f * AnimeState->transparency);

    SDL_Rect dstRect = getBounds();

    if (texture->Size() > 1)
    {
        // 多帧函数
        SDL_Rect srcRect = texture->getSrcRect(AnimeState->frameIndex);
        GFX.RenderCopyEx(texture->texture.get(), &srcRect, &dstRect, AnimeState->angle, NULL, SDL_FLIP_NONE);
        return;
    }
    // 单帧贴图
    GFX.RenderCopyEx(texture->texture.get(), NULL, &dstRect, AnimeState->angle, NULL, SDL_FLIP_NONE);
}
