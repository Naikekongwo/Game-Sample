#include "OpenCore/OpenCore.hpp"
#include "Union.hpp"

SDL_Rect UIElement::getBounds()
{
    if (!AnimeState)
    {
        SDL_Log("UIElement::getBounds() failed: AnimeState is nullptr");
        return SDL_Rect{0, 0, 0, 0};
    }

    const auto &state = *AnimeState;

    // 计算缩放后的实际宽高
    uint16_t renderWidth = static_cast<uint16_t>(nativeWidth * state.scaleX);
    uint16_t renderHeight = static_cast<uint16_t>(nativeHeight * state.scaleY);

    int16_t x = state.PositionX;
    int16_t y = state.PositionY;

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
        SDL_Log("getBounds() invalid anchor: %d", static_cast<uint16_t>(state.Anchor));
        break;
    }

    SDL_Rect logicalRect = {x, y, renderWidth, renderHeight};
    
    // 根据 absolutePosite 属性决定坐标系转换
    if (absolutePosite) {
        // 绝对定位：直接返回在1920x1080逻辑坐标系中的Rect
        return logicalRect;
    } else {
        // 相对定位：将父容器的Bounds视作1920x1080进行坐标转换
        if (parentContainer) {
            SDL_Rect parentBounds = parentContainer->getBounds();
            
            // 将逻辑坐标转换为父容器实际坐标系
            float scaleX = parentBounds.w / 1920.0f;
            float scaleY = parentBounds.h / 1080.0f;
            
            return SDL_Rect{
                parentBounds.x + static_cast<int16_t>(x * scaleX),
                parentBounds.y + static_cast<int16_t>(y * scaleY),
                static_cast<uint16_t>(renderWidth * scaleX),
                static_cast<uint16_t>(renderHeight * scaleY)
            };
        } else {
            // 没有父容器，相对定位等同于绝对定位
            return logicalRect;
        }
    }
}

SDL_Rect UIElement::getRenderedBounds()
{
    return OpenCoreManagers::GFXManager.getScale()->ToScreen(getBounds());
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

bool UIElement::onDestroy()
{
    IDrawableObject::onDestroy();
    
    if(TextureBuffer)
    {
        TextureBuffer.reset();
    }
    return true;
}