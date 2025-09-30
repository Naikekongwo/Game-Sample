#include "OpenCore/OpenCore.hpp"

// 设置动画的播放顺序（sequential 为顺序播放，否则为并行）
void IDrawableObject::setSequential(bool sequential)
{
    AnimeManager->setSequence(sequential);
}

// 设置控件的锚点（用于缩放和旋转）
void IDrawableObject::setAnchor(AnchorPoint anchor)
{
    AnimeState->Anchor = anchor;
}

// 设置控件的位置
void IDrawableObject::setPosition(int x, int y)
{
    AnimeState->PositionX = x;
    AnimeState->PositionY = y;
}

// 动画配置器方法
AnimationPipeline IDrawableObject::Animate()
{
    return AnimationPipeline(AnimeManager.get());
}

// 属性配置器方法
DrawableConfigurator IDrawableObject::Configure()
{
    return DrawableConfigurator(this);
}

// 检查动画是否全部完成
bool IDrawableObject::isAnimeFinished() const
{
    return AnimeManager->isFinished();
}

// 贴图更新方法
void IDrawableObject::changeTexture(Texture* newTexture)
{
    if(newTexture)
    {
        delete this->texture;
        this->texture = newTexture;
    }
    else
    {
        SDL_Log("ImageBoard::changeTexture() encountered a empty texture");
    }
}

