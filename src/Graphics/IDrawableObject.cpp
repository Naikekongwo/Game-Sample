#include "OpenCore/OpenCore.hpp"

void IDrawableObject::setSequential(bool sequential)
{
    AnimeManager->setSequence(sequential);
}

void IDrawableObject::setAnchor(AnchorPoint anchor)
{
    AnimeState->Anchor = anchor;
}

void IDrawableObject::setPosition(int x, int y)
{
    AnimeState->PositionX = x;
    AnimeState->PositionY = y;
}

AnimationPipeline IDrawableObject::Animate()
{
    return AnimationPipeline(AnimeManager.get());
}

DrawableConfigurator IDrawableObject::Configure()
{
    return DrawableConfigurator(this);
}

bool IDrawableObject::isAnimeFinished() const
{
    return AnimeManager->isFinished();
}