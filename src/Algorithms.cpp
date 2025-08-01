// Algorithms.cpp
// 图形算法的实现

#define STB_IMAGE_IMPLEMENTATION
#include "OpenCore/Graphics/GfxCore.h"

SDL_Surface* stbi_loadSurface(const char* filename)
{
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);

    if(!data)
    {
        // 如果加载失败
        SDL_Log("Failed to load image: %s", stbi_failure_reason);
        return nullptr;
    }

    // 四通道掩码
    Uint32 rmask = 0x000000FF;
    Uint32 gmask = 0x0000FF00;
    Uint32 bmask = 0x00FF0000;
    Uint32 amask = 0xFF000000;

    SDL_Surface* result = SDL_CreateRGBSurfaceFrom((void*)data,width,height,32,width*4,rmask,gmask,bmask,amask);

    if(!result)
    {
        // 如果创建失败
        SDL_Log("Failed to create surface: %s", SDL_GetError());

        stbi_image_free(data);

        return nullptr;
    }

    result->userdata = data;

    return result;
}

void FreeStbiSurface(SDL_Surface* surface)
{
    if(surface)
    {
        // 如果表面存在
        if(surface->userdata)
        {
            // 如果 userdata 存在，释放 STB 加载的图像数据
            stbi_image_free(surface->userdata);
        }
        SDL_FreeSurface(surface);
    }
}


SDL_Texture* Algorithms::STB_SDL_LOAD(const std::string& path, SDL_Renderer* renderer)
{
    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    if (!data)
    {
        SDL_Log("STB_SDL_LOAD: Failed to load image %s: %s", path.c_str(), stbi_failure_reason());
        return nullptr;
    }

    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
        data, width, height, 32, width * 4,
        0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);

    if (!surface)
    {
        SDL_Log("STB_SDL_LOAD: Failed to create surface from image %s: %s", path.c_str(), SDL_GetError());
        stbi_image_free(data);
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture)
    {
        SDL_Log("STB_SDL_LOAD: Failed to create texture from surface for image %s: %s", path.c_str(), SDL_GetError());
    }

    SDL_FreeSurface(surface);
    stbi_image_free(data);

    return texture;
}
