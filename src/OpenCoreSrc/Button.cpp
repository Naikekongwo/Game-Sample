
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Animation/AnimationPipeline.hpp"

Button::Button(const std::string &id, uint8_t layer,
               unique_ptr<Texture> texture)
    : UIElement(id, layer, std::move(texture))
{
}

void Button::handlEvents(SDL_Event &event, float totalTime)
{
    SDL_Point mousePos{};
    SDL_Rect bounds = getPhysicalBounds();

    switch (event.type)
    {
    case SDL_MOUSEMOTION:
    {
        mousePos = {event.motion.x, event.motion.y};

        if (!SDL_PointInRect(&mousePos, &bounds))
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
            mousePos = {event.button.x, event.button.y};

            if (SDL_PointInRect(&mousePos, &bounds))
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
            mousePos = {event.button.x, event.button.y};

            if (SDL_PointInRect(&mousePos, &bounds) &&
                State == ButtonState::Pressed)
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
        // 渲染函数
        SDL_SetTextureAlphaMod(texture->texture.get(),
                               255.0f * AnimeState->transparency);

        SDL_Rect dstRect = getLogicalBounds();

        if (texture->Size() > 1)
        {
            // 多帧函数
            SDL_Rect srcRect = texture->getSrcRect(AnimeState->frameIndex);
            GraphicsManager::getInstance().RenderCopyEx(
                texture->texture.get(), &srcRect, &dstRect, AnimeState->angle,
                NULL, SDL_FLIP_NONE);
            return;
        }
        // 单帧贴图
        GraphicsManager::getInstance().RenderCopyEx(
            texture->texture.get(), NULL, &dstRect, AnimeState->angle, NULL,
            SDL_FLIP_NONE);
    }
}
