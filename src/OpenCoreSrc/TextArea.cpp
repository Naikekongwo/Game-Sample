#include "OpenCore/Core/Helpers/Debugger.hpp"
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Animation/IAnimation.hpp"
#include "OpenCore/Runtime/Graphics/IDrawableObject/Texture.hpp"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include <string>

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

    auto GFX = OpenCoreManagers::GFXManager.getInstance();

    Rect dstRect = getLogicalBounds();

    GFX.Draw(m_textureCache, nullptr, &dstRect, 0.0f, nullptr);
    // <渲染逻辑>
}

void TextArea::setText(string_view textContent)
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
    SDL_Texture *textBuffer =
        SDL_CreateTextureFromSurface(GFX.getRenderer(), text);

    if (!textBuffer)
    {
        LOG("Failed to generate text texture");
    }

    SDL_FreeSurface(text);

    // 此处已经知道了目标的尺寸为loRect.w x loRect.h

    int texW, texH;

    SDL_QueryTexture(textBuffer, nullptr, nullptr, &texW, &texH);

    Rect dstRect = {0, 0, m_fontSize * (texW * 1.0f / texH), m_fontSize * 1.0f};

    if (m_shadowEnable)
    {
        SDL_Surface *shadowSurface = TTF_RenderUTF8_Blended(
            font, m_textContent.c_str(), {0, 0, 0, transparency});

        SDL_Texture *shadowBuffer =
            SDL_CreateTextureFromSurface(GFX.getRenderer(), shadowSurface);

        Rect shadowRect = dstRect;
        shadowRect.x += m_shadowOffset;
        shadowRect.y += m_shadowOffset;

        GFX.Draw(shadowBuffer, nullptr, &shadowRect, 0.0f, nullptr);

        SDL_FreeSurface(shadowSurface);
        SDL_DestroyTexture(shadowBuffer);
    }

    GFX.Draw(textBuffer, nullptr, &dstRect, 0.0f, nullptr);

    SDL_DestroyTexture(textBuffer);

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

void TextArea::setShadow(bool enableTag, int shadowOffset)
{
    this->m_shadowEnable = enableTag;

    this->m_shadowOffset =
        (shadowOffset <= 0 | shadowOffset > 255) ? 5 : shadowOffset;

    LOG("调整了阴影选项，当前状态为{}，阴影位移为 {}",
        (enableTag) ? "开" : "关", m_shadowOffset);
}
