#pragma once

#include <SDL2/SDL.h>
#include <memory>
using std::shared_ptr;

struct Texture
{
    // 行数和列数
    size_t xCount = 1;
    size_t yCount = 1;

    // 贴图的大小
    uint16_t width, height;

    // 材质
    shared_ptr<SDL_Texture> texture;

    Texture(size_t x, size_t y, shared_ptr<SDL_Texture> tex);

    uint16_t Size() const noexcept { return xCount * yCount; }

    float getWidthHeight() const noexcept
    {
        return static_cast<float>(width) / static_cast<float>(height);
    }

    SDL_Rect getSubRect(size_t index);
    SDL_Texture *get() const { return texture.get(); }
};