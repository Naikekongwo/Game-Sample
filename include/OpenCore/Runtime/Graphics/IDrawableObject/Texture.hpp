#pragma once

#include "OpenCore/Core/Math/OpenCore_Rect.hpp"
#include <SDL2/SDL.h>
#include <memory>
using std::shared_ptr;

struct Texture
{
    /**
     * @brief 常规纹理构造器
     *
     * @param x
     * @param y
     * @param tex
     */
    Texture(size_t x, size_t y, std::shared_ptr<SDL_Texture> tex);

    /**
     * @brief 构造自带纹理的Texture
     *
     */
    Texture();

    // 行数和列数
    size_t xCount = 1;
    size_t yCount = 1;

    // 贴图的大小
    uint16_t width, height;

    // 材质
    std::shared_ptr<SDL_Texture> texture;

    // 隐式转换为 SDL_Texture*
    operator SDL_Texture *() const { return texture.get(); }

    uint16_t Size() const noexcept { return xCount * yCount; }

    float getWidthHeight() const noexcept
    {
        return static_cast<float>(width) / static_cast<float>(height);
    }

    SDL_Rect getSubRect(size_t index);
    Rect getSubRect(size_t startIndex, size_t endIndex);
    SDL_Texture *get() const { return texture.get(); }

    /**
     * @brief 重新设置纹理属性
     *
     * @param rows
     * @param cols
     * @param texture
     * @return true
     * @return false
     */
    bool configure(size_t rows, size_t cols,
                   std::shared_ptr<SDL_Texture> texture);
};