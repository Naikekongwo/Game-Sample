#include "OpenCore/OpenCore.hpp"
#include <memory>

Texture::Texture(size_t x, size_t y, shared_ptr<SDL_Texture> tex)
    : xCount(x), yCount(y), texture(tex)
{
    int W, H;

    if (!texture)
        LOG("Texture::Texture() encountered empty texture in the "
            "initialization.");

    SDL_QueryTexture(texture.get(), NULL, NULL, &W, &H);

    width = static_cast<uint16_t>(W);
    height = static_cast<uint16_t>(H);

    if (xCount == 0 || yCount == 0)
    {
        LOG("Texture::Texture() Invalid xCount or yCount value.");
        return;
    }

    width /= xCount;
    height /= yCount;
}

Texture::Texture()
{
    width = 1;
    height = 1;

    xCount = 1;
    yCount = 1;
}

bool Texture::configure(size_t rows, size_t cols,
                        shared_ptr<SDL_Texture> texture)
{
    if (texture == nullptr)
        return false;

    int W, H;

    if (!texture)
        LOG("部署时候遇到空的纹理，该方法不应该传入空纹理");

    this->texture = texture;

    SDL_QueryTexture(texture.get(), NULL, NULL, &W, &H);

    width = static_cast<uint16_t>(W);
    height = static_cast<uint16_t>(H);

    if (xCount == 0 || yCount == 0)
    {
        LOG("纹理网格参数非法，其不应为0 宽高: {} {}", xCount, yCount);
        return false;
    }

    width /= xCount;
    height /= yCount;

    return true;
}

SDL_Rect Texture::getSubRect(size_t index)
{
    // 构造一个0矩阵的常量，避免重复生成
    static const SDL_Rect emptyRect{0, 0, 0, 0};
    if (index < 0 || index >= Size())
    {
        LOG("Texture::getSubRect() index out of range: {}", index);
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