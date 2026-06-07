/**
 * @file TextArea.hpp
 * @author your name (you@domain.com)
 * @brief 文本框体
 * @version 0.1
 * @date 2026-04-25
 *
 * @copyright Copyright (c) 2026
 *
 */

#pragma once

#include "OpenCore/Runtime/Animation/IAnimation.hpp"
#include "OpenCore/Runtime/Graphics/IDrawableObject/UIElement.hpp"

#include <memory>
#include <string>

using std::string;
using std::unique_ptr;

/**
 * @class TextArea
 * @brief 文本框体类
 *
 */
class TextArea : public UIElement
{
  public:
    TextArea(const string &id, uint8_t layer, short fontID);
    ~TextArea() override;

    void handlEvents(SDL_Event &event, float totalTime) override;
    bool onDestroy() override;

    void Draw() override;
    /**
     * @brief 设置文本框显示的内容
     *
     * @param textContent
     */
    void setText(std::string_view textContent);

    /**
     * @brief 设置字体大小
     *
     * @param fontSize
     */
    void setFontSize(short fontSize);
    void onUpdate(float totalTime) override;
    void setShadow(bool enableTag, int shadowOffset);

    void alignCenter(bool tag) { m_aligncenter = tag; }

    void setTextColor(uint8_t r, uint8_t g, uint8_t b)
    {
        m_colorR = r;
        m_colorG = g;
        m_colorB = b;
        m_valid = false;
    }

  private:
    short fontID = 9001;
    short m_fontSize = 20;

    uint8_t m_colorR = 255;
    uint8_t m_colorG = 255;
    uint8_t m_colorB = 255;

    bool m_shadowEnable = true;
    bool m_aligncenter = false;

    bool m_valid = true;
    uint8_t m_shadowOffset = 2;
    uint8_t transparency = 176;

    SDL_Texture *m_textureCache;
    string m_textContent;

    /**
     * @brief 重新刷新纹理缓存
     *
     */
    void refreshTextureCache();
};