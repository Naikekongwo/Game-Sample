#pragma once

#include "OpenCore/Runtime/Animation/IAnimation.hpp"
#include "OpenCore/Runtime/Animation/Manager/AnimationManager.hpp"
#include <memory>

using std::unique_ptr;

class AnimationPipeline;

class CollectionAnimation : public IAnimation
{
  public:
    // 构造函数
    CollectionAnimation(AnimationPipeline *parentPipeline,
                        bool isParalle = true);

    void onUpdate(float totalTime, VisualState &state) override; // 更新动画
    bool isFinished() const override;                            // 是否结束
    void reset(float totalTime, VisualState &state) override;    // 重置
    bool isLoop() override { return isLooping; }

    AnimationPipeline *Begin();

  private:
    //   Parallel : true 代表动画组顺序执行
    bool Parallel = true;
    bool isLooping = false;
    bool finished = false;
    unique_ptr<AnimationManager> AnimeManager;
    AnimationPipeline *parentPipe = nullptr;
};