#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"
#include <memory>
CollectionAnimation::CollectionAnimation(MyAnimationPipeline *parentPipeline,
                                         bool isParalle)
{
    AnimeManager = std::make_unique<AnimationManager>();
    this->parentPipe = parentPipeline;

    Parallel = isParalle;
}

MyAnimationPipeline *CollectionAnimation::Begin()
{
    return new MyAnimationPipeline(AnimeManager.get(), parentPipe);
}

void CollectionAnimation::onUpdate(float totalTime, AnimationState &state)
{
    AnimeManager->onUpdate(totalTime, state);
}

bool CollectionAnimation::isFinished() const
{
    return AnimeManager->isFinished();
}

void CollectionAnimation::reset(float totalTime, AnimationState &state)
{
    AnimeManager->reset(totalTime, state);
}