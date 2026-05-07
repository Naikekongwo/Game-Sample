#include "OpenCore/Core/Helpers/Debugger.hpp"
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Animation/IAnimation.hpp"
#include "OpenCore/Runtime/Graphics/IDrawableObject/Texture.hpp"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <memory>

TextArea::TextArea(const string &id, uint8_t layer, short fontID)
    : UIElement(id, layer, nullptr)
{
    this->fontID = fontID;
    LOG("文本框创建，字体代号:{}", fontID);
}

void TextArea::Draw()
{
    // 检查是否需要显示
    if (m_textContent.empty())
    {
        return;
    }
    // 检查缓存
    if (!m_textContent.empty() && m_textureCache == nullptr)
    {
        refreshTextureCache();
    }
    if (VState->getAlpha() <= 0.0f)
    {
        // 透明的元素无需渲染
        return;
    }

    // <渲染逻辑>

    Rect dstRect = getLogicalBounds();

    auto GFX = OpenCoreManagers::GFXManager.getInstance();

    GFX.Draw(m_textureCache, nullptr, &dstRect, 0.0f, nullptr);
    // <渲染逻辑>
}

void TextArea::setText(const string &textContent)
{
    // 设置后刷新缓存
    m_textContent = textContent;
    refreshTextureCache();
}

void TextArea::setFontSize(short fontSize)
{
    // 设置字号后刷新缓存
    m_fontSize = fontSize;
    refreshTextureCache();
}

void TextArea::refreshTextureCache()
{
    // 刷新缓存
    Rect loRect = getLogicalBounds();
    auto &GFX = GraphicsManager::getInstance();

    auto target = GFX.createTexture(loRect.w, loRect.h);

    if (!target)
        return;

    GFX.setRenderTarget(target);

    // <渲染字体的具体方法>
    auto font = OpenCoreManagers::ResManager.GetFont(fontID);
    if (!font)
    {
        LOG("资源管理器查询字体对象的结果为空");
        return;
    }

    SDL_Surface *text = TTF_RenderUTF8_Blended(font, m_textContent.c_str(),
                                               {255, 255, 255, 255});

    SDL_Texture *textures =
        SDL_CreateTextureFromSurface(GFX.getRenderer(), text);

    if (!textures)
    {
        LOG("Failed to generate text texture");
    }

    // 此处已经知道了目标的尺寸为loRect.w x loRect.h

    int texW, texH;

    SDL_QueryTexture(textures, nullptr, nullptr, &texW, &texH);

    Rect dstRect = {0, 0, loRect.h * (texW * 1.0f / texH), loRect.h};

    GFX.Draw(textures, nullptr, &dstRect, 0.0f, nullptr);

    SDL_DestroyTexture(textures);
    SDL_FreeSurface(text);

    // <渲染字体的具体方法>
    GFX.setRenderTarget(nullptr);

    m_textureCache = target;
}

void TextArea::handlEvents(SDL_Event &event, float totalTime)
{
    switch (event.type)
    {
    case SDL_WINDOWEVENT:
    {
        if (event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            refreshTextureCache();
        }
    }
    default:
        break;
    }
}