#pragma once

#include "OpenCore/Animation/IAnimation.hpp"
#include "OpenCore/Animation/Manager/AnimationManager.hpp"
#include <memory>

class MyAnimationPipeline;

class CollectionAnimation : public IAnimation
{
  public:
    // 构造函数
    CollectionAnimation(MyAnimationPipeline *parentPipeline,
                        bool isParalle = true);

    void onUpdate(float totalTime, AnimationState &state) override; // 更新动画
    bool isFinished() const override;                               // 是否结束
    void reset(float totalTime, AnimationState &state) override;    // 重置
    bool isLoop() override { return isLooping; }

    MyAnimationPipeline *Begin();

  private:
    //   Parallel : true 代表动画组顺序执行
    bool Parallel = true;
    bool isLooping = false;
    bool finished = false;
    std::unique_ptr<AnimationManager> AnimeManager;
    MyAnimationPipeline *parentPipe = nullptr;
};