
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

    AnimeState->frameIndex = stateIndex;
}
void Button::onRender()
{
    if (texture->texture)
    {
        auto Graphics = GraphicsManager::getInstance();

        SDL_SetTextureAlphaMod(texture->get(), AnimeState->getAlpha());

        Rect dRect = getLogicalBounds();

        auto frameIndex = (AnimeState->getFrameIndex() > texture->Size())
                              ? 0
                              : AnimeState->getFrameIndex();

        Rect sRect = texture->getSubRect(frameIndex);

        Graphics.Draw(texture->get(), &sRect, &dRect, AnimeState->getAngle(),
                      NULL);
    }
}
