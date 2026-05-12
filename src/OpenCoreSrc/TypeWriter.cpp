#include "OpenCore/Runtime/Graphics/UI/TypeWriter.hpp"
#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"
#include "OpenCore/Runtime/Animation/IAnimation.hpp"
#include "OpenCore/Runtime/Animation/Manager/AnimationManager.hpp"
#include "OpenCore/Runtime/Graphics/IDrawableObject/UIElement.hpp"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <cstdint>
#include <memory>

TypeWriter::TypeWriter(string_view id, uint8_t layer, short fontID)
    : UIElement(id.data(), layer, nullptr)
{
    // ID LAYER 和纹理都已经设置

    this->VState = std::make_unique<VisualState>();
    this->AnimeManager = std::make_unique<AnimationManager>();

    this->fontID = fontID;

    LOG("初始化成功，ID {}, 字体ID {}", id.data(), fontID);
}

void TypeWriter::Draw()
{
    if (VState->getAlpha() <= 0.0f)
        return;

    if (!m_textureCache || m_textureValid == false)
        return;

    if (status < TypeWriterStatus::Ready)
        return;

    // <真正的绘制逻辑>

    auto &GFX = OpenCoreManagers::GFXManager;

    Rect dstRect = getLogicalBounds();

    // 在此处处理打字机效果
    GFX.Draw(m_textureCache, nullptr, &dstRect, 0.0f, nullptr);

    // <真正的绘制逻辑>
}

void TypeWriter::handlEvents(SDL_Event &event, float totalTime)
{
    switch (event.type)
    {
    case SDL_WINDOWEVENT:
    {
        if (event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            m_textureValid = false;
        }
    }
    default:
        break;
    }
}

void TypeWriter::setText(string_view textContent)
{
    // 设置对应的文字
    m_textContent = textContent;
    m_textureValid = false;
}

void TypeWriter::setFontSize(short fontSize)
{
    m_fontSize = fontSize;
    m_textureValid = false;
}

void TypeWriter::onUpdate(float totalTime)
{
    if (!m_textureValid)
    {
        generateTexture(nullptr);
    }
}

void TypeWriter::setShadow(bool enableTag, int shadowOffset)
{
    m_shadowEnable = enableTag;
    m_shadowOffset = shadowOffset;
    m_textureValid = false;
}

bool TypeWriter::generateTexture(SDL_Texture *texture)
{
    m_parsedLines.clear();

    auto &GFX = OpenCoreManagers::GFXManager.getInstance();
    Rect container = getLogicalBounds();

    auto font = OpenCoreManagers::ResManager.getInstance().GetFont(fontID);
    if (!font)
        return false;

    m_textureCache = GFX.createTexture(container.w, container.h);
    if (!m_textureCache)
        return false;

    GFX.setRenderTarget(m_textureCache);

    SDL_SetRenderDrawBlendMode(GFX.getRenderer(), SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(GFX.getRenderer(), 0, 0, 0, 0);
    SDL_RenderClear(GFX.getRenderer());

    auto utf8Len = [](unsigned char c) -> size_t
    {
        if ((c & 0x80) == 0x00)
            return 1;
        if ((c & 0xE0) == 0xC0)
            return 2;
        if ((c & 0xF0) == 0xE0)
            return 3;
        return 4;
    };

    auto measure = [&](const std::string &s) -> float
    {
        SDL_Surface *surf =
            TTF_RenderUTF8_Blended(font, s.c_str(), {255, 255, 255, 255});
        if (!surf)
            return 0;

        float scale = m_fontSize * 1.0f / surf->h;
        float w = surf->w * scale;

        SDL_FreeSurface(surf);
        return w;
    };

    std::string line;

    auto pushLine = [&]()
    {
        m_parsedLines.push_back(line);
        line.clear();
    };

    for (size_t i = 0; i < m_textContent.size();)
    {
        size_t len = utf8Len((unsigned char)m_textContent[i]);
        std::string ch = m_textContent.substr(i, len);

        if (ch == "\n")
        {
            pushLine();
            i += len;
            continue;
        }

        std::string test = line + ch;

        if (!line.empty() && measure(test) > container.w)
        {
            pushLine();
            continue;
        }

        line = test;
        i += len;
    }

    if (!line.empty())
        pushLine();

    int y = 0;

    for (auto &l : m_parsedLines)
    {
        SDL_Surface *surf =
            TTF_RenderUTF8_Blended(font, l.c_str(), {255, 255, 255, 255});

        if (!surf)
        {
            GFX.setRenderTarget(nullptr);
            return false;
        }

        SDL_Texture *tex =
            SDL_CreateTextureFromSurface(GFX.getRenderer(), surf);

        if (!tex)
        {
            SDL_FreeSurface(surf);
            GFX.setRenderTarget(nullptr);
            return false;
        }

        float scale = m_fontSize * 1.0f / surf->h;

        Rect dst;
        dst.w = surf->w * scale;
        dst.h = m_fontSize;
        dst.x = m_aligncenter ? (container.w - dst.w) * 0.5f : 0;
        dst.y = y;

        if (y + dst.h > container.h)
        {
            SDL_DestroyTexture(tex);
            SDL_FreeSurface(surf);
            break;
        }

        if (m_shadowEnable)
        {
            Rect s = dst;
            s.x += m_shadowOffset;
            s.y += m_shadowOffset;

            SDL_SetTextureColorMod(tex, 0, 0, 0);
            SDL_SetTextureAlphaMod(tex, 180);
            GFX.Draw(tex, nullptr, &s, 0.0f, nullptr);

            SDL_SetTextureColorMod(tex, 255, 255, 255);
            SDL_SetTextureAlphaMod(tex, 255);
        }

        GFX.Draw(tex, nullptr, &dst, 0.0f, nullptr);

        SDL_DestroyTexture(tex);
        SDL_FreeSurface(surf);

        y += m_fontSize + lineGap;
    }

    GFX.setRenderTarget(nullptr);

    m_textureValid = true;
    status = TypeWriterStatus::Ready;

    return true;
}