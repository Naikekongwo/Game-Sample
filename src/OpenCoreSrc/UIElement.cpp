
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Animation/IAnimation.hpp"
#include "OpenCore/Runtime/Animation/Manager/AnimationManager.hpp"

SDL_Rect UIElement::getLogicalBounds()
{
    if (!VState)
    {
        LOG("UIElement::getLogicalBounds() failed: VState is nullptr");
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

SDL_Rect UIElement::getPhysicalBounds() { return getLogicalBounds(); }

bool UIElement::onDestroy()
{
    IDrawableObject::onDestroy();
    return true;
}

UIElement::UIElement(const string &id, short layer, unique_ptr<Texture> texture)
{
    this->id = id;
    this->layer = layer;

    if (!texture)
    {
        LOG("UIElement id {} encountered a empty texture, but relax, for some "
            "element have child element it's ok to init without texture.",
            id.c_str());
    }
    else
    {
        this->texture = std::move(texture);
    }
}