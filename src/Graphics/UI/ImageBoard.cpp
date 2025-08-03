#include "OpenCore/OpenCore.h"

ImageBoard::ImageBoard(short id, short layer, std::unique_ptr<Texture> texture)
{
    // 创建一个专属的动画状态
    AnimeState = std::make_unique<AnimationState>();

    // 设置ID 层级属性
    this->id = id;
    this->layer = layer;

    // 创建动画管理器
    AnimeManager = std::make_unique<AnimationManager>();

    // 获取材质
    if(!texture) SDL_Log("ImageBoard::ImageBoard() encountered a empty texture.");

    this->texture = std::move(texture);
}


void ImageBoard::handlEvents(SDL_Event &event, float totalTime)
{
    // 图片框处理事件的类
    // 图片框无需处理事件
}

SDL_Rect ImageBoard::getBounds() {
    if (!AnimeState) {
        SDL_Log("ImageBoard::getRenderRect() failed: AnimeState is nullptr");
        return SDL_Rect{0, 0, 0, 0};
    }

    const auto& state = *AnimeState;

    // 计算缩放后的实际宽高
    int renderWidth  = static_cast<int>(bWidth  * state.scaleX);
    int renderHeight = static_cast<int>(bHeight * state.scaleY);

    int x = state.PositionX;
    int y = state.PositionY;

    // 按锚点偏移位置
    switch (state.Anchor) {
        case AnchorPoint::TopLeft:                          break;
        case AnchorPoint::TopCenter:     x -= renderWidth / 2;                  break;
        case AnchorPoint::TopRight:      x -= renderWidth;                      break;

        case AnchorPoint::MiddleLeft:                      y -= renderHeight / 2; break;
        case AnchorPoint::Center:         x -= renderWidth / 2; y -= renderHeight / 2; break;
        case AnchorPoint::MiddleRight:    x -= renderWidth;    y -= renderHeight / 2; break;

        case AnchorPoint::BottomLeft:                     y -= renderHeight;     break;
        case AnchorPoint::BottomCenter:  x -= renderWidth / 2; y -= renderHeight;     break;
        case AnchorPoint::BottomRight:   x -= renderWidth;    y -= renderHeight;     break;

        default:
            SDL_Log("getRenderRect() invalid anchor: %d", static_cast<int>(state.Anchor));
            break;
    }

    return SDL_Rect{ x, y, renderWidth, renderHeight };
}


void ImageBoard::onUpdate(float totalTime)
{
    AnimeManager->onUpdate(totalTime, *AnimeState);
    // 更新动画！
}

void ImageBoard::onRender(SDL_Renderer* render)
{
    // 渲染函数
    SDL_SetTextureAlphaMod(texture.get()->texture, 255.0f * AnimeState->transparency);

    SDL_Rect dstRect = getBounds();

    if(texture->Size() > 1)
    {
        // 多帧函数
        SDL_Rect srcRect = texture->getSrcRect(AnimeState->frameIndex);
        SDL_RenderCopyEx(render, texture.get()->texture, &srcRect, &dstRect, AnimeState->angle, NULL,SDL_FLIP_NONE);
    }
    // 单帧贴图
    SDL_RenderCopyEx(render, texture.get()->texture, NULL, &dstRect, AnimeState->angle, NULL,SDL_FLIP_NONE);
}