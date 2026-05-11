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

    // <真正的绘制逻辑>

    auto &GFX = OpenCoreManagers::GFXManager;

    Rect dstRect = getLogicalBounds();

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
    // 该方法目前只设计空参数
    // if (m_textureCache)
    // {
    //     // 如果当前的纹理不为空
    //     SDL_DestroyTexture(m_textureCache);
    //     m_textureCache = nullptr;
    // }

    m_parsedLines.clear();

    Rect containerSize = getLogicalBounds();
    auto &GFX = OpenCoreManagers::GFXManager.getInstance();

    m_textureCache = GFX.createTexture(containerSize.w, containerSize.h);

    if (!m_textureCache)
    {
        LOG("获取渲染目标纹理时出现了问题");
        return false;
    }

    // 设置渲染目标
    GFX.setRenderTarget(m_textureCache);

    auto font = OpenCoreManagers::ResManager.getInstance().GetFont(fontID);

    int w, h;

    int lineGap = 2;
    int lineStartIndex = 0;
    int lineEndIndex = 0;

    // 解析文本
    bool should_stop = false;
    while (!should_stop)
    {
        if (lineEndIndex >= m_textContent.length())
        {
            should_stop = true;
            m_parsedLines.push_back(m_textContent.substr(
                lineStartIndex, lineEndIndex - lineStartIndex));
            break;
        }
        if (m_textContent[lineEndIndex] == '\n')
        {
            m_parsedLines.push_back(m_textContent.substr(
                lineStartIndex, lineEndIndex - lineStartIndex));
            lineStartIndex = lineEndIndex + 1;
            lineEndIndex++;
            continue;
        }
        TTF_SizeText(
            font,
            m_textContent
                .substr(lineStartIndex, lineEndIndex - lineStartIndex + 1)
                .c_str(),
            &w, &h);

        float factor = m_fontSize * 1.0f / h;

        if (w * factor > containerSize.w)
        {
            // 超长了
            m_parsedLines.push_back(m_textContent.substr(
                lineStartIndex, lineEndIndex - lineStartIndex));
            lineStartIndex = lineEndIndex;
            lineEndIndex++;
            continue;
        }
        lineEndIndex++;
    }

    if (mode == TypeWriterMode::TypeWriter)
    {
        int offsetY = 0;
        // 一般打字机模式
        for (int i = 0; i < m_parsedLines.size(); i++)
        {
            SDL_Surface *textLine = TTF_RenderUTF8_Blended(
                font, m_parsedLines[i].c_str(), {255, 255, 255, 255});
            if (!textLine)
            {
                LOG("生成表面时失败");
                return false;
            }
            SDL_Texture *textTexture =
                SDL_CreateTextureFromSurface(GFX.getRenderer(), textLine);
            if (!textTexture)
            {
                LOG("转换表面时失败");
                SDL_FreeSurface(textLine);
                return false;
            }

            int texW, texH;
            SDL_QueryTexture(textTexture, nullptr, nullptr, &texW, &texH);
            if (offsetY + m_fontSize > containerSize.h)
            {
                // 已经超出了，这一行无需渲染了
                SDL_FreeSurface(textLine);
                SDL_DestroyTexture(textTexture);
                break;
            }

            // 没有超出，我们继续渲染
            Rect dstRect;
            dstRect.h = m_fontSize;
            dstRect.w = (texW * 1.0f / texH) * m_fontSize;
            dstRect.x = 0;
            dstRect.y = offsetY;

            GFX.Draw(textTexture, nullptr, &dstRect, 0.0f, nullptr);

            SDL_FreeSurface(textLine);
            SDL_DestroyTexture(textTexture);

            offsetY += m_fontSize + lineGap;
        }
        m_textureValid = true;
    }

    // 还原
    GFX.setRenderTarget(nullptr);

    return true;
}