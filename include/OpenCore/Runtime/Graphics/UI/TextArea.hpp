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

    void handlEvents(SDL_Event &event, float totalTime) override {};

    void Draw() override;

    /**
     * @brief 设置文本框显示的内容
     *
     * @param textContent
     */
    void setText(const string &textContent);

    /**
     * @brief 设置字体大小
     *
     * @param fontSize
     */
    void setFontSize(short fontSize);

    void onUpdate(float totalTime) override {};

  private:
    short fontID = 9001;
    short m_fontSize = 20;

    unique_ptr<Texture> m_textureCache;
    string m_textContent;

    /**
     * @brief 重新刷新纹理缓存
     *
     */
    void refreshTextureCache();
};