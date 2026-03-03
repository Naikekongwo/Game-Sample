#include "OpenCore/OpenCore.hpp"

Texture::Texture(size_t x, size_t y, shared_ptr<SDL_Texture> tex)
    : xCount(x), yCount(y), texture(tex)
{
    int W, H;

    if (!texture)
        Console_Log("Texture::Texture() encountered empty texture in the "
                    "initialization.");

    SDL_QueryTexture(texture.get(), NULL, NULL, &W, &H);

    width = static_cast<uint16_t>(W);
    height = static_cast<uint16_t>(H);

    if (xCount == 0 || yCount == 0)
    {
        Console_Log("Texture::Texture() Invalid xCount or yCount value.");
        return;
    }

    width /= xCount;
    height /= yCount;
}

SDL_Rect Texture::getSubRect(size_t index)
{
    // 构造一个0矩阵的常量，避免重复生成
    static const SDL_Rect emptyRect{0, 0, 0, 0};
    if (index < 0 || index >= Size())
    {
        Console_Log("Texture::getSubRect() index out of range: %d", index);
        return emptyRect;
    }

    if (Size() == 1)
    {
        // 一张大图
        return SDL_Rect{0, 0, width, height};
    }
    else
    {
        uint8_t col = index % xCount;
        uint8_t row = index / xCount;

        return SDL_Rect{col * width, row * height, width, height};
    }
}