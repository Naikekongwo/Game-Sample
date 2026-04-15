
#include "OpenCore/OpenCore.hpp"
#include <cstdint>


void IDrawableObject::setSequential(bool sequential)
{
    AnimeManager->setSequence(sequential);
}


void IDrawableObject::onUpdate(float totalTime)
{
    AnimeManager->onUpdate(totalTime, *VState);
}


void IDrawableObject::handlEvents(SDL_Event &event, float totalTime)
{
    // 空置
}


void IDrawableObject::setAnchor(AnchorPoint anchor) { VState->Anchor = anchor; }


void IDrawableObject::setPosition(float xPercent, float yPercent)
{
    SDL_Rect parentRect{0, 0, OpenCoreManagers::SetManager.getTargetWidth(),
                        OpenCoreManagers::SetManager.getTargetHeight()};

    if (parentContainer != nullptr)
    {
        parentRect = parentContainer->getLogicalBounds();
    }

    VState->Position[0] = parentRect.x + xPercent * parentRect.w;
    VState->Position[1] = parentRect.y + parentRect.h * yPercent;
}

SDL_Rect IDrawableObject::getPhysicalBounds()
{
    return (magnetFactor == 0) ? getLogicalBounds()
                               : magnetRect(getLogicalBounds());
}

SDL_Rect IDrawableObject::getLogicalBounds()
{
    if (!VState)
    {
        Console_Log("UIElement::getLogicalBounds() failed: VState is nullptr");
        return SDL_Rect{0, 0, 0, 0};
    }

    const auto &state = *VState;

    
    float logicalWidth = absWidth * state.scale[0];
    float logicalHeight = absHeight * state.scale[1];

    float logicalX = state.Position[0];
    float logicalY = state.Position[1];

    
    switch (state.Anchor)
    {
    case AnchorPoint::TopLeft:
        break;
    case AnchorPoint::TopCenter:
        logicalX -= logicalWidth * 0.5f;
        break;
    case AnchorPoint::TopRight:
        logicalX -= logicalWidth;
        break;
    case AnchorPoint::MiddleLeft:
        logicalY -= logicalHeight * 0.5f;
        break;
    case AnchorPoint::Center:
        logicalX -= logicalWidth * 0.5f;
        logicalY -= logicalHeight * 0.5f;
        break;
    case AnchorPoint::MiddleRight:
        logicalX -= logicalWidth;
        logicalY -= logicalHeight * 0.5f;
        break;
    case AnchorPoint::BottomLeft:
        logicalY -= logicalHeight;
        break;
    case AnchorPoint::BottomCenter:
        logicalX -= logicalWidth * 0.5f;
        logicalY -= logicalHeight;
        break;
    case AnchorPoint::BottomRight:
        logicalX -= logicalWidth;
        logicalY -= logicalHeight;
        break;
    }

    
    return SDL_Rect{static_cast<int>(std::round(logicalX)),
                    static_cast<int>(std::round(logicalY)),
                    static_cast<int>(std::round(logicalWidth)),
                    static_cast<int>(std::round(logicalHeight))};
}


void IDrawableObject::setScale(float w, float h)
{
    SDL_Rect parentRect{0, 0, OpenCoreManagers::SetManager.getTargetWidth(),
                        OpenCoreManagers::SetManager.getTargetHeight()};

    if (parentContainer)
    {
        parentRect = parentContainer->getLogicalBounds();
    }

    float wph = 1.0f;
    if (w * h == 0.0f)
    {
        // texture一定要存在才能得到原始尺寸！
        if (!texture->texture)
        {
            Console_Log(
                "Element id %s was a texture less element, so you shouldn't "
                "use WIDTH/HEIGHT-still in this element.",
                id.c_str());
            return;
        }

        wph = texture->getWidthHeight();
        // Ensure that not both parameters are zero!
        absWidth = (w == 0.0f) ? h * parentRect.h * wph : parentRect.w * w;
        absHeight = (h == 0.0f) ? (w * parentRect.w) / wph : parentRect.h * h;

        Console_Log("Still id %s, %d, %d, %f", id.c_str(), absWidth, absHeight,
                    wph);
    }
    else
    {
        
        absWidth = w * parentRect.w;
        absHeight = h * parentRect.h;
    }
}


AnimationPipeline IDrawableObject::Animate()
{
    // return AnimationPipeline(AnimeManager.get());
    // 上面是系统管线
    return AnimationPipeline(AnimeManager.get(), nullptr);
}


DrawableConfigurator IDrawableObject::Configure()
{
    return DrawableConfigurator(this);
}


bool IDrawableObject::isAnimeFinished() const
{
    return AnimeManager->isFinished();
}


void IDrawableObject::changeTexture(unique_ptr<Texture> newTexture)
{
    if (newTexture)
    {
        texture = std::move(newTexture);
    }
    else
    {
        Console_Log("ImageBoard::changeTexture() encountered a empty texture");
    }
}

IDrawableObject::IDrawableObject()
{
    this->id = "null";
    this->layer = 0;

    AnimeManager = std::make_unique<AnimationManager>();
    VState = std::make_unique<VisualState>();
}

void IDrawableObject::setParentContainer(IDrawableObject *parentContainer)
{
    if (parentContainer != nullptr)
    {
        
        absolutePosite = false;
        this->parentContainer = parentContainer;

        
        this->layer = parentContainer->getLayer() + 1;
    }
    else
    {
        absolutePosite = true;
    }
}

void IDrawableObject::setTransparency(float alpha)
{
    VState->transparency = alpha;
}

bool IDrawableObject::onDestroy()
{
    AnimeManager.reset();
    VState.reset();

    texture.reset();

    return true;
}

void IDrawableObject::setMagnetFactor(int Margin) { magnetFactor = Margin; }

SDL_Rect IDrawableObject::magnetRect(const SDL_Rect &srcRect) const
{
    
    if (magnetFactor == 0)
        return srcRect;

    auto &GFX = GraphicsManager::getInstance();

    int winW, winH;
    SDL_GetWindowSize(GFX.getWindow(), &winW, &winH);

    int centerX = winW / 2;
    int centerY = winH / 2;

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    
    float normX = (mouseX - centerX) / static_cast<float>(centerX);
    float normY = (mouseY - centerY) / static_cast<float>(centerY);

    
    normX = std::clamp(normX, -1.0f, 1.0f);
    normY = std::clamp(normY, -1.0f, 1.0f);

    
    float offsetX = normX * magnetFactor;
    float offsetY = normY * magnetFactor;

    SDL_Rect result = srcRect;
    result.x += static_cast<int>(offsetX);
    result.y += static_cast<int>(offsetY);

    return result;
}
