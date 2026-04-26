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

    Rect dstRect = getLogicalBounds();

    auto GFX = OpenCoreManagers::GFXManager.getInstance();

    GFX.Draw(m_textureCache->get(), nullptr, &dstRect, 0.0f, 0.0f);
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

    GFX.setOffScreenRender(target);

    int texW, texH;
    SDL_QueryTexture(texture->get(), nullptr, nullptr, &texW, &texH);

    Rect srcRect{};
    Rect dstRect{};

    int targetW, targetH;
    SDL_QueryTexture(target, nullptr, nullptr, &targetW, &targetH);

    // <渲染字体的具体方法>
    auto font = OpenCoreManagers::ResManager.GetFont(fontID);
    if (!font)
    {
        LOG("资源管理器查询字体对象的结果为空");
    }

    // <渲染字体的具体方法>

    GFX.setOffScreenRender(nullptr);
}