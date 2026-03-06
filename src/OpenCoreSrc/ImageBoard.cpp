
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

void ImageBoard::onRender() { Draw(); }

void ImageBoard::Draw()
{
    auto &GFX = OpenCoreManagers::GFXManager.getInstance();

    Rect VRect = GFX.getSccissorRect();
    Rect dstRect = getLogicalBounds();
    // 在此处不需要进行PhysicalBounds的判断，如果其逻辑位置在屏幕之外，那就没有必要渲染。

    // 确认可见度
    if (VState->getAlpha() > 0.0f && texture->get() && visible(dstRect, VRect))
    {
        SDL_SetTextureAlphaMod(texture->texture.get(), VState->getAlpha());

        Rect srcRect = texture->getSubRect(VState->getFrameIndex());

        dstRect = magnetRect(dstRect);

        GFX.Draw(texture->texture.get(), &srcRect, &dstRect, VState->getAngle(),
                 NULL);
    }
}
