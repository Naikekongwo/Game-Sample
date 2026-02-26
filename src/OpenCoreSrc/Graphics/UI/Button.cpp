#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/Animation/AnimationPipeline.hpp"
#include "OpenCore/OpenCore.hpp"

Button::Button(const std::string &id, uint8_t layer,
               std::unique_ptr<Texture> texture)
{

    // 设置ID 层级属性
    this->id = id;
    this->layer = layer;

    // 获取材质
    if (!texture)
    {
        SDL_Log("Button::Button() encountered a empty texture.");
        return;
        // 如果材质为空，那么我们直接强制返回
    }

    this->texture = std::move(texture);
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
                State = ButtonState::Pressed;
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
