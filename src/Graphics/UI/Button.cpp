#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Animation/AnimationPipeline.hpp"

Button::Button(std::string id, short layer, Texture* texture)
{
    // 创建一个专属的动画状态
    AnimeState = std::make_unique<AnimationState>();

    // 设置ID 层级属性
    this->id = id;
    this->layer = layer;

    // 创建动画管理器
    AnimeManager = std::make_unique<AnimationManager>();

    // 获取材质
    if (!texture)
    {
        SDL_Log("Button::Button() encountered a empty texture.");
        return;
        // 如果材质为空，那么我们直接强制返回
    }

    this->texture = texture;
}

void Button::handlEvents(SDL_Event &event, float totalTime)
{
    // 主要是判断按钮状态：悬停与否
    switch(event.type)
    {
        case SDL_MOUSEMOTION:
        {
            SDL_Point mousePos = { event.motion.x, event.motion.y };
            SDL_Rect bounds = getBounds();
            if(!SDL_PointInRect(&mousePos, &bounds))
            {
                // 如果鼠标不在其范围之内
                State = ButtonState::Normal;
            }
            else
            {
                State = (State==ButtonState::Pressed) ? ButtonState::Pressed : ButtonState::Hovered;
            }
            break;
        }
        case SDL_MOUSEBUTTONDOWN:
        {
            if(State == ButtonState::Hovered)
            {
                State = ButtonState::Pressed;
            }
            else if(State == ButtonState::Pressed)
            {
                State = ButtonState::Pressed;
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
