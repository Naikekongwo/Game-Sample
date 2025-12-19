#include "OpenCore/OpenCore.hpp"
#include "Eclipsea/Eclipsea.hpp"

OpenCore_Rect UIElement::getBounds()
{
    if (!AnimeState)
    {
        SDL_Log("UIElement::getBounds() failed: AnimeState is nullptr");
        return OpenCore_Rect{0, 0, 0, 0};
    }

    const auto &state = *AnimeState;

    // 计算基于1920x1080逻辑坐标系的宽高
    uint16_t logicalWidth = static_cast<uint16_t>(nativeWidth * state.scale[0]);
    uint16_t logicalHeight = static_cast<uint16_t>(nativeHeight * state.scale[1]);

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

    // 如果是绝对定位，直接返回逻辑坐标
    if (absolutePosite || !parentContainer) {
        return OpenCore_Rect{logicalX, logicalY, logicalWidth, logicalHeight};
    }

    // 相对定位：基于父容器的实际边界进行转换
    OpenCore_Rect parentBounds = parentContainer->getBounds();
    
    
    // 将本元素的逻辑坐标转换到父容器的实际坐标系中
    // Position 是基于父容器1920x1080逻辑空间的
    // Scale 也是基于父容器1920x1080逻辑空间的
    return OpenCore_Rect{
        parentBounds.x + static_cast<int16_t>(logicalX),
        parentBounds.y + static_cast<int16_t>(logicalY),
        static_cast<uint16_t>(logicalWidth),  // 宽度也会根据父容器缩放
        static_cast<uint16_t>(logicalHeight)  // 高度也会根据父容器缩放
    };
}


OpenCore_Rect UIElement::getRenderedBounds()
{
    return OpenCoreManagers::GFXManager.getScale()->ToScreen(getBounds());
}

void UIElement::onRender()
{
    auto& GFX = OpenCoreManagers::GFXManager.getInstance();


    // 虽然这个渲染方法是虚函数
    // 但是UIElement必须作为基类给出一个默认的实现

    OpenCore_Rect Borders = getBounds();

    if(directRender)
    {
        // 如果预烘焙是开启的
        if(!TextureBuffer.get())
        {
            // 创建新的缓冲贴图
            TextureBuffer.reset(GFX.createTexture(Borders.w, Borders.h));
            
            preRenderTexture(TextureBuffer.get()); 
        }
        GFX.RenderTile(*TextureBuffer.get(), nullptr, &Borders, {0.0f, NULL, 0});
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