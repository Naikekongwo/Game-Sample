#include "OpenCore/OpenCore.hpp"
#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/Animation/AnimationPipeline.hpp"

Button::Button(const std::string& id, uint8_t layer, std::unique_ptr<Texture> texture)
{

    // 设置ID 层级属性
    this->id = id;
    this->layer = layer;

    this->directRender = false;

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
    SDL_Point mousePos = { event.motion.x, event.motion.y };
    SDL_Rect bounds = getRenderedBounds();
    // 主要是判断按钮状态：悬停与否
    switch(event.type)
    {
        case SDL_MOUSEMOTION:
        {
            
            if(!SDL_PointInRect(&mousePos, &bounds))
            {
                // 如果鼠标不在其范围之内
                State = ButtonState::Normal;
            }
            else
            {
                State = (State == ButtonState::Pressed) ? ButtonState::Pressed : ButtonState::Hovered;
            }
            break;
        }
        case SDL_MOUSEBUTTONDOWN:
        {
            if (event.button.button == SDL_BUTTON_LEFT) {
                if (SDL_PointInRect(&mousePos, &bounds))
                {
                    State = ButtonState::Pressed;
                }
            }
            break;
        }
        case SDL_MOUSEBUTTONUP:
        {
            if (event.button.button == SDL_BUTTON_LEFT) {
                
                // 检查鼠标是否在按钮范围内且之前是按下的状态
                if (SDL_PointInRect(&mousePos, &bounds) && State == ButtonState::Pressed)
                {
                    // 触发点击回调
                    if (onClick) {
                        onClick();
                    }
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
    AnimeState->frameIndex = State;
}

void Button::onRender()
{
    // 渲染函数
    SDL_SetTextureAlphaMod(texture->texture.get(), 255.0f * AnimeState->transparency);

    SDL_Rect dstRect = getBounds();

    if (texture->Size() > 1)
    {
        // 多帧函数
        SDL_Rect srcRect = texture->getSrcRect(AnimeState->frameIndex);
        GraphicsManager::getInstance().RenderCopyEx(texture->texture.get(), &srcRect, &dstRect, AnimeState->angle, NULL, SDL_FLIP_NONE);
        return;
    }
    // 单帧贴图
    GraphicsManager::getInstance().RenderCopyEx(texture->texture.get(), NULL, &dstRect, AnimeState->angle, NULL, SDL_FLIP_NONE);
}
