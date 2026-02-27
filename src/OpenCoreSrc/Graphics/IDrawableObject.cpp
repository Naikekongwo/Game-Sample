#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"

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
void IDrawableObject::setPosition(float xPercent, float yPercent)
{
    uint16_t baseW;
    uint16_t baseH;

    if (parentContainer == nullptr)
    {
        baseW = BASE_WINDOW_WIDTH;
        baseH = BASE_WINDOW_HEIGHT;
    }
    else
    {
        SDL_Rect parent = parentContainer->getLogicalBounds();
        baseW = parent.w;
        baseH = parent.h;
    }

    if (xPercent != 0 && yPercent != 0)
    {
        // 二者都不为0，直接按照父尺寸赋值
        AnimeState->Position[0] = baseW * xPercent;
        AnimeState->Position[1] =
            ((parentContainer == nullptr) ? baseW : baseH) * yPercent;
        // 父对象不是屏幕就可以分别系数
    }
    else if (xPercent == 0)
    {
        // 以y之百分比进行尺寸标定
        AnimeState->Position[1] =
            ((parentContainer == nullptr) ? baseW : baseH) * yPercent;
        AnimeState->Position[0] =
            AnimeState->Position[1] *
            (texture->getFrameWidth() / texture->getFrameHeight());
    }
    else if (yPercent == 0)
    {
        // 以x之百分比进行尺寸标定
        AnimeState->Position[0] =
            ((parentContainer == nullptr) ? baseW : baseH) * xPercent;
        AnimeState->Position[1] =
            AnimeState->Position[0] *
            (texture->getFrameHeight() / texture->getFrameWidth());
    }
    else
    {
        // 二者都为0
        SDL_Log("Encountered Fatal error in element scaling!");
        // 以y之百分比进行尺寸标定
        AnimeState->Position[1] = 1;
        AnimeState->Position[0] = 1;
    }
}

SDL_Rect IDrawableObject::getPhysicalBounds()
{
    return (magnetFactor == 0) ? getLogicalBounds()
                               : magnetRect(getLogicalBounds());
}

// 设置控件的大小
void IDrawableObject::setScale(float w, float h)
{
    uint16_t baseW;
    uint16_t baseH;

    if (parentContainer == nullptr)
    {
        // 相对于屏幕
        baseW = BASE_WINDOW_WIDTH;
        baseH = BASE_WINDOW_HEIGHT;
    }
    else
    {
        SDL_Rect parent = parentContainer->getLogicalBounds();
        baseW = parent.w;
        baseH = parent.h;
    }

    if (w != 0 && h != 0)
    {
        // 二者都不为0，直接按照父尺寸赋值
        absWidth = baseW * w;
        absHeight = ((parentContainer == nullptr) ? baseW : baseH) * h;
        // 父对象不是屏幕就可以分别系数
    }
    else if (w == 0)
    {
        // 以y之百分比进行尺寸标定
        absHeight = ((parentContainer == nullptr) ? baseW : baseH) * h;
        absWidth =
            absHeight * (texture->getFrameWidth() / texture->getFrameHeight());
    }
    else if (h == 0)
    {
        // 以x之百分比进行尺寸标定
        absWidth = ((parentContainer == nullptr) ? baseW : baseH) * w;
        absHeight =
            absWidth * (texture->getFrameHeight() / texture->getFrameWidth());
    }
    else
    {
        // 二者都为0
        SDL_Log("Encountered Fatal error in element scaling!");
        // 以y之百分比进行尺寸标定
        absHeight = 1;
        absWidth = 1;
    }

    absWidth = baseW * w;
    absHeight = baseW * h;
}

// 动画配置器方法
MyAnimationPipeline IDrawableObject::Animate()
{
    // return AnimationPipeline(AnimeManager.get());
    // 上面是系统管线
    return MyAnimationPipeline(AnimeManager.get(), nullptr);
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
void IDrawableObject::changeTexture(unique_ptr<Texture> newTexture)
{
    if (newTexture)
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

void IDrawableObject::setParentContainer(IDrawableObject *parentContainer)
{
    if (parentContainer != nullptr)
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

void IDrawableObject::setTransparency(float alpha)
{
    AnimeState->transparency = alpha;
}

bool IDrawableObject::onDestroy()
{
    AnimeManager.reset();
    AnimeState.reset();

    texture.reset();

    return true;
}

void IDrawableObject::setMagnetFactor(int Margin) { magnetFactor = Margin; }

SDL_Rect IDrawableObject::magnetRect(const SDL_Rect &srcRect) const
{
    // 如果不启用视差，直接返回
    if (magnetFactor == 0)
        return srcRect;

    auto &GFX = GraphicsManager::getInstance();

    int winW, winH;
    SDL_GetWindowSize(GFX.getWindow(), &winW, &winH);

    int centerX = winW / 2;
    int centerY = winH / 2;

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    // 归一化到 -1 ~ 1
    float normX = (mouseX - centerX) / static_cast<float>(centerX);
    float normY = (mouseY - centerY) / static_cast<float>(centerY);

    // 防止鼠标移出窗口导致过大
    normX = std::clamp(normX, -1.0f, 1.0f);
    normY = std::clamp(normY, -1.0f, 1.0f);

    // 计算偏移
    float offsetX = normX * magnetFactor;
    float offsetY = normY * magnetFactor;

    SDL_Rect result = srcRect;
    result.x += static_cast<int>(offsetX);
    result.y += static_cast<int>(offsetY);

    return result;
}