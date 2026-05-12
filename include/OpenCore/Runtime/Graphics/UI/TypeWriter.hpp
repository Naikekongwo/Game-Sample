/**
 * @file TypeWriter.hpp
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
#include "OpenCore/Runtime/Graphics/UI/BaseBackground.hpp"

#include <memory>
#include <string>
#include <vector>

using std::string;
using std::string_view;
using std::unique_ptr;
using std::vector;

enum class TypeWriterMode
{
    TypeWriter,
    Flowing,
};

enum class TypeWriterStatus
{
    Creating,
    Ready,
    Flowing,
    Finished
};

/**
 * @class TypeWriter
 * @brief 文本框体类
 *
 */
class TypeWriter : public UIElement
{
  public:
    TypeWriter(string_view id, uint8_t layer, short fontID);
    ~TypeWriter() override;

    void handlEvents(SDL_Event &event, float totalTime) override;
    bool onDestroy() override;

    void Draw() override;
    /**
     * @brief 设置文本框显示的内容
     *
     * @param textContent
     */
    void setText(std::string_view textContent);
    string getText() { return m_textContent; }

    /**
     * @brief 设置字体大小
     *
     * @param fontSize
     */
    void setFontSize(short fontSize);

    /**
     * @brief 更新的方法
     *
     * @param totalTime
     */
    void onUpdate(float totalTime) override;

    /**
     * @brief 设置阴影的相关属性
     *
     * @param enableTag
     * @param shadowOffset
     */
    void setShadow(bool enableTag, int shadowOffset);

    /**
     * @brief 设置对齐相关参数
     *
     * @param tag
     */
    void alignCenter(bool tag) { m_aligncenter = tag; }

    bool generateTexture(SDL_Texture *texture) override;

  private:
    short fontID = 9001;
    short m_fontSize = 20;
    short lineGap = 2;

    SDL_Color fontColor{255, 255, 255, 255};

    bool m_textureValid = false;
    bool m_shadowEnable = true;
    bool m_aligncenter = false;

    uint8_t m_shadowOffset = 2;
    uint8_t transparency = 176;

    SDL_Texture *m_textureCache;
    string m_textContent;

    vector<string> m_parsedLines;

    TypeWriterStatus status = TypeWriterStatus::Creating;
    TypeWriterMode mode = TypeWriterMode::TypeWriter;

    unique_ptr<BaseBackground> m_baseBackground;
};