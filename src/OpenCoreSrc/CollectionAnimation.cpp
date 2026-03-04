
#include "OpenCore/OpenCore.hpp"
#include <memory>

CollectionAnimation::CollectionAnimation(AnimationPipeline *parentPipeline,
                                         bool isParalle)
{
    AnimeManager = std::make_unique<AnimationManager>();
    this->parentPipe = parentPipeline;

    Parallel = isParalle;
}

AnimationPipeline *CollectionAnimation::Begin()
{
    return new AnimationPipeline(AnimeManager.get(), parentPipe);
}

void CollectionAnimation::onUpdate(float totalTime, VisualState &state)
{
    AnimeManager->onUpdate(totalTime, state);
}

bool CollectionAnimation::isFinished() const
{
    return AnimeManager->isFinished();
}

void CollectionAnimation::reset(float totalTime, VisualState &state)
{
    AnimeManager->reset(totalTime, state);
}