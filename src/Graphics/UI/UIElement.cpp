#include "OpenCore/OpenCore.hpp"

SDL_Rect UIElement::getBounds()
{
    if (!AnimeState)
    {
        SDL_Log("ImageBoard::getRenderRect() failed: AnimeState is nullptr");
        return SDL_Rect{0, 0, 0, 0};
    }

    const auto &state = *AnimeState;

    // 计算缩放后的实际宽高
    int renderWidth = static_cast<int>(nativeWidth * state.scaleX);
    int renderHeight = static_cast<int>(nativeHeight * state.scaleY);

    int x = state.PositionX;
    int y = state.PositionY;

    // 按锚点偏移位置
    switch (state.Anchor)
    {
    case AnchorPoint::TopLeft:
        break;
    case AnchorPoint::TopCenter:
        x -= renderWidth / 2;
        break;
    case AnchorPoint::TopRight:
        x -= renderWidth;
        break;

    case AnchorPoint::MiddleLeft:
        y -= renderHeight / 2;
        break;
    case AnchorPoint::Center:
        x -= renderWidth / 2;
        y -= renderHeight / 2;
        break;
    case AnchorPoint::MiddleRight:
        x -= renderWidth;
        y -= renderHeight / 2;
        break;

    case AnchorPoint::BottomLeft:
        y -= renderHeight;
        break;
    case AnchorPoint::BottomCenter:
        x -= renderWidth / 2;
        y -= renderHeight;
        break;
    case AnchorPoint::BottomRight:
        x -= renderWidth;
        y -= renderHeight;
        break;

    default:
        SDL_Log("getRenderRect() invalid anchor: %d", static_cast<int>(state.Anchor));
        break;
    }

    return SDL_Rect{x, y, renderWidth, renderHeight};
}

void UIElement::onRender()
{
    auto& GFX = OpenCoreManagers::GFXManager.getInstance();


    // 虽然这个渲染方法是虚函数
    // 但是UIElement必须作为基类给出一个默认的实现

    SDL_Rect Borders = getBounds();

    if(directRender)
    {
        // 如果预烘焙是开启的
        if(!TextureBuffer.get())
        {
            // 创建新的缓冲贴图
            TextureBuffer.reset(GFX.createTexture(Borders.w, Borders.h));
            
            preRenderTexture(TextureBuffer.get()); 
        }
        GFX.RenderCopyEx(TextureBuffer.get(), nullptr, &Borders, 0.0f, nullptr, SDL_FLIP_NONE);
    }
    else
    {
        // directRender 关闭，使用实时渲染
        preRenderTexture(nullptr);
    }
}