#include "OpenCore/OpenCore.hpp"
#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/Animation/AnimationPipeline.hpp"

ImageBoard::ImageBoard(const std::string& id, uint8_t layer, std::unique_ptr<Texture> texture)
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

    this->texture = std::move(texture);
}


void ImageBoard::onRender()
{
    auto& GFX = GraphicsManager::getInstance();
    // 渲染函数


    texture->texture.get()->setAlpha(255.0f * AnimeState->transparency);

    OpenCore_Rect dstRect = getBounds();

    if (texture->Size() > 1)
    {
        // 多帧函数
        OpenCore_Rect srcRect = texture->getSrcRect(AnimeState->frameIndex);
        GFX.RenderTile(*texture->texture.get(), &srcRect, &dstRect, {0.0f, nullptr, 0});
        return;
    }
    // 单帧贴图
    GFX.RenderTile(*texture->texture.get(), NULL, &dstRect, {0.0f, nullptr, 0});
}
