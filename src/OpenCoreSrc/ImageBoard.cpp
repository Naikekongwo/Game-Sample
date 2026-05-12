
#include "OpenCore/Runtime/Graphics/UI/ImageBoard.hpp"
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Animation/AnimationPipeline.hpp"

ImageBoard::ImageBoard(const std::string &id, uint8_t layer,
                       unique_ptr<Texture> texture)
    : UIElement(id, layer, std::move(texture))
{
    // id、layer、texture、AnimeManager、VState 均已在基类链中正确初始化
}

void ImageBoard::Draw()
{
    auto &GFX = OpenCoreManagers::GFXManager.getInstance();

    Rect VRect = GFX.getSccissorRect();
    Rect dstRect = getLogicalBounds();
    ///< 在此处不需要进行PhysicalBounds的判断，如果其逻辑位置在屏幕之外，那就没有必要渲染。
    if (VState->getAlpha() > 0.0f && texture->get() && visible(dstRect, VRect))
    {
        uint8_t alpha = VState->getAlpha();
        SDL_SetTextureAlphaMod(texture->get(), alpha);

        Rect srcRect = texture->getSubRect(VState->getFrameIndex());

        dstRect = magnetRect(dstRect);

        GFX.Draw(texture->get(), &srcRect, &dstRect, VState->getAngle(), NULL);
    }
}

void ImageBoard::onUpdate(float totalTime)
{
    if (!isAnimeFinished())
    {
        AnimeManager->onUpdate(totalTime, *VState.get());
    }
}