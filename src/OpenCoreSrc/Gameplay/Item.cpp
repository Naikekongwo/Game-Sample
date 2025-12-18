#include "Eclipsea/Eclipsea.hpp"
#include "OpenCore/OpenCore.hpp"


Item::Item(uint16_t id, const std::string &name,
           std::unique_ptr<Texture> texture)
{
    if (!texture)
    {
        SDL_Log(
            "Item::Item() encountered a empty texture when initialize item.");
    }

    if (name.empty())
    {
        SDL_Log("Item::Item() encountered a empty name when initialize item.");
    }

    if (texture)
    {
        this->texture_ = std::move(texture);
    }
}

void Item::setTexture(std::unique_ptr<Texture> texture)
{
    texture_ = std::move(texture);
}