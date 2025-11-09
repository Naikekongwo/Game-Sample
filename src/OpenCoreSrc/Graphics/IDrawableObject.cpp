#include "OpenCore/OpenCore.hpp"
#include "Eclipsea/Eclipsea.hpp"

// 设置动画的播放顺序（sequential 为顺序播放，否则为并行）
void IDrawableObject::setSequential(bool sequential)
{
    AnimeManager->setSequence(sequential);
}

// 默认的onUpdate方法，即更新动画状态
void IDrawableObject::onUpdate(float totalTime)
{
    AnimeManager->onUpdate(totalTime, *AnimeState);
}

// 默认的handlEvents方法，执行空状态
void IDrawableObject::handlEvents(SDL_Event &event, float totalTime)
{
    // 空置
}

// 设置控件的锚点（用于缩放和旋转）
void IDrawableObject::setAnchor(AnchorPoint anchor)
{
    AnimeState->Anchor = anchor;
}

// 设置控件的位置
void IDrawableObject::setPosition(int16_t x, int16_t y)
{
    AnimeState->PositionX = x;
    AnimeState->PositionY = y;
}

// 动画配置器方法
MyAnimationPipeline IDrawableObject::Animate()
{
    // return AnimationPipeline(AnimeManager.get());
    // 上面是系统管线
    return MyAnimationPipeline(AnimeManager.get());
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
void IDrawableObject::changeTexture(std::unique_ptr<Texture> newTexture)
{
    if(newTexture)
    {
        texture = std::move(newTexture);
    }
    else
    {
        SDL_Log("ImageBoard::changeTexture() encountered a empty texture");
    }
}


IDrawableObject::IDrawableObject()
{
    this->id = "null";
    this->layer = 0;
    
    AnimeManager = std::make_unique<AnimationManager>();
    AnimeState = std::make_unique<AnimationState>();
}


void IDrawableObject::setParentContainer(IDrawableObject* parentContainer)
{
    if(parentContainer!=nullptr)
    {
        // 有父容器
        absolutePosite = false;
        this->parentContainer = parentContainer;

        // 向上堆叠
        this->layer = parentContainer->getLayer() + 1;
    }
    else
    {
        absolutePosite = true;
    }
}

bool IDrawableObject::onDestroy()
{
    AnimeManager.reset();
    AnimeState.reset();

    texture.reset();

    return true;
}