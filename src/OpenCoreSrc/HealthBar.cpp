#include "OpenCore/Runtime/Graphics/Sprite/HealthBar.hpp"
#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"
#include <cmath>

void HealthBar::onEnter() { m_healthPercent = make_shared<float>(0.5f); }

void HealthBar::Draw()
{
    if (VState->getAlpha() <= 0.0f)
        return;

    if (!texture)
    {
        LOG("空纹理");
        return;
    }

    // <渲染逻辑开始>

    Rect dstRect = getLogicalBounds();
    Rect frameRect = texture->getSubRect(0);
    Rect valueRect = texture->getSubRect(1);

    auto GFX = OpenCoreManagers::GFXManager.getInstance();

    // 渲染血量
    Rect cutRect = dstRect;
    // auto health = m_healthPercent.lock();
    // if (!health)
    // {
    //     LOG("血量绑定的对象已经悬空 ID {}", id);
    //     return;
    // }
    cutRect.x += dstRect.w * (1 - 0.5f);
    // x向后推
    cutRect.w = (0.5f) * dstRect.w;
    // w缩小

    // 渲染血量
    GFX.Draw(texture->get(), &valueRect, &cutRect, 0.0f, nullptr);

    // 渲染框架
    GFX.Draw(texture->get(), &frameRect, &dstRect, 0.0f, nullptr);
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