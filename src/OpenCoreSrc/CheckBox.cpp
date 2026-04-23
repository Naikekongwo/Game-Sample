
#include "OpenCore/OpenCore.hpp"
#include <SDL2/SDL_events.h>

CheckBox::CheckBox(const string &id, short layer, unique_ptr<Texture> texture)
    : UIElement(id, layer, std::move(texture))
{
    
}

void CheckBox::refreshStatus()
{
    if (Img)
    {
        Img->getVisualState()->frameIndex = (*Value) ? 1 : 0;
    }
}

void CheckBox::Draw()
{
    if (Img)
    {
        refreshStatus();
        Img->Draw();
    }
}

void CheckBox::handlEvents(SDL_Event &event, float totalTime)
{
    if (status == CheckBoxStatus::Creating)
    {
        onEnter();
    }
    else
    {
        SDL_Point mousePos{};
        SDL_Rect bounds = getPhysicalBounds();

        switch (event.type)
        {
            
        case SDL_MOUSEBUTTONDOWN:
        {
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                mousePos = {event.button.x, event.button.y};

                if (SDL_PointInRect(&mousePos, &bounds) &&
                    status == CheckBoxStatus::Ready)
                {
                    
                    status = CheckBoxStatus::Checking;
                }
            }
            break;
        }

        case SDL_MOUSEBUTTONUP:
        {
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                if (status == CheckBoxStatus::Checking)
                {
                    *Value = (*Value) ? false : true;
                    status = CheckBoxStatus::Ready;
                    refreshStatus();
                }
            }
            break;
        }

        default:
            break;
        }
    }
}

void CheckBox::onEnter()
{
    if (status == CheckBoxStatus::Creating)
    {
        Img = std::make_unique<ImageBoard>("checkimg", 1,
                                           std::move(this->texture));

        Img->Configure()
            .Parent(this)
            .Anchor(AnchorPoint::Center)
            .Posite(0.5f, 0.5f)
            .Scale(1.0f, 1.0f);

        refreshStatus();
        status = CheckBoxStatus::Ready;
    }
}

void CheckBox::onExit() {}

bool CheckBox::onDestroy()
{
    Value.reset();
    Img.reset();

    return true;
}