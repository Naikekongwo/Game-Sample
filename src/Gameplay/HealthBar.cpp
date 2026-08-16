#include "Eclipsea/Gameplay/Sprite/HealthBar.hpp"
#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore.hpp"
#include <cmath>
#include <memory>

void HealthBar::onEnter() {}

void HealthBar::Draw()
{
    if (VState->getAlpha() <= 0.0f)
        return;

    if (!texture)
    {
        LOG("空纹理");
        return;
    }

    if (!m_healthPercent)
    {
        m_healthPercent = std::make_shared<float>(1.0f);
    }

    // <渲染逻辑开始>

    Rect dstRect = getLogicalBounds();

    Rect frameRect = texture->getSubRect(0);
    Rect valueRect = texture->getSubRect(1);
    Rect wellRect = texture->getSubRect(2);
    Rect goodRect = texture->getSubRect(3);

    // 渲染血量
    Rect cutRect = dstRect;
    // auto health = *m_healthPercent.lock();
    // if (!health)
    // {
    //     LOG("血量绑定的对象已经悬空 ID {}", id);
    //     return;
    // }
    cutRect.x += dstRect.w * (1 - *m_healthPercent);
    // x向后推
    cutRect.w = (*m_healthPercent) * dstRect.w;
    // w缩小

    // 渲染血量（OpenCore 26.2：绘制收归 Texture::Draw）
    if (*m_healthPercent < 0.5)
    {
        texture->Draw(&valueRect, &cutRect, 0.0, nullptr);
    }
    else if (*m_healthPercent < 0.7)
    {
        texture->Draw(&wellRect, &cutRect, 0.0, nullptr);
    }
    else
    {
        texture->Draw(&goodRect, &cutRect, 0.0, nullptr);
    }

    // 渲染框架
    texture->Draw(&frameRect, &dstRect, 0.0, nullptr);
}

HealthBar::HealthBar(string_view id, short layer, unique_ptr<Texture> texture)
{
    this->id = "HealthBar";
    this->layer = 0;

    this->VState = std::make_unique<VisualState>();
    this->AnimeManager = std::make_unique<AnimationManager>();

    if (!texture)
    {
        LOG("初始化失败，不允许空的纹理! ID {}", id);
        return;
    }

    this->texture = std::move(texture);

    // 初始化成功了
}