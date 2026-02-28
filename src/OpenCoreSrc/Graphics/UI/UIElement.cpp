#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"

SDL_Rect UIElement::getLogicalBounds()
{
    if (!AnimeState)
    {
        SDL_Log("UIElement::getLogicalBounds() failed: AnimeState is nullptr");
        return SDL_Rect{0, 0, 0, 0};
    }

    const auto &state = *AnimeState;

    // 计算基于1920x1080逻辑坐标系的宽高
    uint16_t logicalWidth = static_cast<uint16_t>(absWidth * state.scale[0]);
    uint16_t logicalHeight = static_cast<uint16_t>(absHeight * state.scale[1]);

    int16_t logicalX = state.Position[0];
    int16_t logicalY = state.Position[1];

    // 按锚点偏移位置（在1920x1080逻辑坐标系中）
    switch (state.Anchor)
    {
    case AnchorPoint::TopLeft:
        break;
    case AnchorPoint::TopCenter:
        logicalX -= logicalWidth / 2;
        break;
    case AnchorPoint::TopRight:
        logicalX -= logicalWidth;
        break;
    case AnchorPoint::MiddleLeft:
        logicalY -= logicalHeight / 2;
        break;
    case AnchorPoint::Center:
        logicalX -= logicalWidth / 2;
        logicalY -= logicalHeight / 2;
        break;
    case AnchorPoint::MiddleRight:
        logicalX -= logicalWidth;
        logicalY -= logicalHeight / 2;
        break;
    case AnchorPoint::BottomLeft:
        logicalY -= logicalHeight;
        break;
    case AnchorPoint::BottomCenter:
        logicalX -= logicalWidth / 2;
        logicalY -= logicalHeight;
        break;
    case AnchorPoint::BottomRight:
        logicalX -= logicalWidth;
        logicalY -= logicalHeight;
        break;
    }

    return SDL_Rect{logicalX, logicalY, logicalWidth, logicalHeight};
}

SDL_Rect UIElement::getPhysicalBounds() { return getLogicalBounds(); }

bool UIElement::onDestroy()
{
    IDrawableObject::onDestroy();
    return true;
}