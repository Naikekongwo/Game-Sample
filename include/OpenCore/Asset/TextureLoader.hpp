#pragma once

// TextureLoader.hpp
// 纹理加载工具

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>

#include <memory>
#include <string>

using std::string;
using std::unique_ptr;

struct TextureDeleter
{
    void operator()(SDL_Texture *texture) const;
    void operator()(TTF_Font *font) const;
};

using TexturePtr = unique_ptr<SDL_Texture, TextureDeleter>;
using FontPtr = unique_ptr<TTF_Font, TextureDeleter>;

inline SDL_Surface *LoadSurface(const string &path)
{
    SDL_Surface *surface = IMG_Load(path.c_str());

    if (!surface)
    {
        SDL_Log("IMG_Load failed: %s", IMG_GetError());
        return nullptr;
    }
    SDL_Surface *convertedSurface =
        SDL_ConvertSurfaceFormat(surface,
                                 SDL_PIXELFORMAT_ABGR8888, // 或其他需要的格式
                                 0);

    SDL_FreeSurface(surface);

    if (!convertedSurface)
    {
        SDL_Log("SDL_ConvertSurfaceFormat failed: %s", SDL_GetError());
        return nullptr;
    }

    SDL_Log("ResourceManager::LoadSurface() %s surface loaded successfully.",
            path.c_str());
    return convertedSurface;
}

inline TexturePtr ConvertSurfaceToTexture(SDL_Renderer *renderer,
                                          SDL_Surface *surface)
{
    if (!renderer)
    {
        SDL_Log("ResourceManager::ConvertSurfaceToTexture: renderer is null");
        SDL_FreeSurface(surface);
        return nullptr;
    }

    // 创建纹理
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture)
    {
        SDL_Log("SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
        return nullptr;
    }

    return TexturePtr(texture);
}