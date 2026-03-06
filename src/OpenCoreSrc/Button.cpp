
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Animation/AnimationPipeline.hpp"

#include "OpenCore/Core/Math/OpenCore_Rect.hpp"

Button::Button(const std::string &id, uint8_t layer,
               unique_ptr<Texture> texture)
    : UIElement(id, layer, std::move(texture))
{
}

void Button::handlEvents(SDL_Event &event, float totalTime)
{
    Point mousePos{};
    Rect bounds = getPhysicalBounds();

    switch (event.type)
    {
    case SDL_MOUSEMOTION:
    {
        mousePos.x = event.motion.x;
        mousePos.y = event.motion.y;

        if (!PointInRect(mousePos, bounds))
            State = ButtonState::Normal;
        else
            State = (State == ButtonState::Pressed) ? ButtonState::Pressed
                                                    : ButtonState::Hovered;
        break;
    }

    case SDL_MOUSEBUTTONDOWN:
    {
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            mousePos.x = event.button.x;
            mousePos.y = event.button.y;

            if (PointInRect(mousePos, bounds))
            {
                OpenCoreManagers::SFXManager.playSE(1002);
                State = ButtonState::Pressed;
            }
        }
        break;
    }

    case SDL_MOUSEBUTTONUP:
    {
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            mousePos.x = event.button.x;
            mousePos.y = event.button.y;

            if (PointInRect(mousePos, bounds) && State == ButtonState::Pressed)
            {
                if (onClick)
                    onClick();

                State = ButtonState::Hovered;
            }
            else
            {

                State = ButtonState::Normal;
            }
        }
        break;
    }

    default:
        break;
    }
}

void Button::onUpdate(float totalTime)
{
    if (!texture)
        return;
    int maxFrame = texture->Size();

    int stateIndex = static_cast<int>(State);

    if (stateIndex >= maxFrame)
        stateIndex = 0;

    VState->frameIndex = stateIndex;
}
void Button::onRender() { Draw(); }

void Button::Draw()
{
    auto &GFX = OpenCoreManagers::GFXManager.getInstance();

    Rect VRect = GFX.getSccissorRect();
    Rect dstRect = getLogicalBounds();

    if (texture->get() && visible(dstRect, VRect) && VState->getAlpha() > 0.0f)
    {
        SDL_SetTextureAlphaMod(texture->get(), VState->getAlpha());

        auto frameIndex = (VState->getFrameIndex() > texture->Size())
                              ? 0
                              : VState->getFrameIndex();

        Rect srcRect = texture->getSubRect(frameIndex);

        GFX.Draw(texture->get(), &srcRect, &dstRect, VState->getAngle(), NULL);
    }
}