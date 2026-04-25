#include "OpenCore/OpenCore.hpp"

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

    // <渲染逻辑>
}

void TextArea::setText(const string &textContent)
{
    // 设置后刷新缓存
    m_textContent = textContent;
    refreshTextureCache();
}