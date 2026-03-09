
#include "OpenCore/OpenCore.hpp"
#include <memory>

Scrollbar::Scrollbar(const string &id, short layer, short backTexID,
                     short buttTexID)
    : UIElement(id, layer, nullptr)
{

    if (backTexID * buttTexID)
    {
        this->backgroundTexture = backTexID;
        this->buttonTexture = buttTexID;
    }

    value = make_shared<float>(0);
}

void Scrollbar::onEnter()
{
    if (status == ScrollStatus::Creating)
    {
        // 避免反复初始化
        baseBack =
            UI<BaseBackground>("background", 1, backgroundTexture, NULL, NULL);
        slideBar = UI<ImageBoard>("slideimg", 1, buttonTexture, 1, 1);

        // 部署
        slideBar->Configure()
            .Anchor(AnchorPoint::Center)
            .Parent(this)
            .Posite(0.5f, 0.5f)
            .Scale(0.0f, 1.2f)
            .Sequence(true);

        baseBack->Configure()
            .Parent(this)
            .Posite(0.5f, 0.5f)
            .Scale(1.0f, 1.0f)
            .Sequence(true)
            .Anchor(AnchorPoint::Center);

        baseBack->setNativeScale(10);
        baseBack->onEnter();
        slideBar->onEnter();

        UpdateBar();

        status = ScrollStatus::Ready;
    }
}

void Scrollbar::Draw()
{
    if (slideBar && baseBack)
    {
        // 都存在才会绘制
        baseBack->Draw();
        slideBar->Draw();
    }
}

void Scrollbar::onUpdate(float totalTime)
{
    if (slideBar)
    {
        slideBar->onUpdate(totalTime);
    }
}

void Scrollbar::UpdateBar() { slideBar->setPosition(*value, 0.5f); }

void Scrollbar::handlEvents(SDL_Event &event, float totalTime)
{
    if (baseBack)
        baseBack->handlEvents(event, totalTime);
    // 处理点击事件
    SDL_Point mousePos{};
    SDL_Rect bounds = getPhysicalBounds();

    switch (event.type)
    {
        // 鼠标运动时的事件
    case SDL_MOUSEMOTION:
    {
        mousePos = {event.motion.x, event.motion.y};
        if (!SDL_PointInRect(&mousePos, &bounds))
        {
            status = ScrollStatus::Ready;
        }

        if (status == ScrollStatus::Following)
        // 运动时离开控件
        {
            *value = (mousePos.x - bounds.x) / static_cast<float>(bounds.w);
            UpdateBar();
        }
    }

    case SDL_MOUSEBUTTONDOWN:
    {
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            mousePos = {event.button.x, event.button.y};

            if (SDL_PointInRect(&mousePos, &bounds))
            {
                // 触发拖动条件
                status = ScrollStatus::Following;
            }
        }
        break;
    }

    case SDL_MOUSEBUTTONUP:
    {
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            if (status == ScrollStatus::Following)
            {
                status = ScrollStatus::Ready;
            }
        }
        break;
    }

    default:
        break;
    }
}

bool Scrollbar::onDestroy() { return true; }

void Scrollbar::onExit()
{
    slideBar.reset();
    baseBack.reset();
    value.reset();
}