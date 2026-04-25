/**
 * @file TextureLoader.hpp
 * @brief 纹理加载工具模块。
 *
 * 提供从文件加载表面(SDL_Surface)、将表面转换为纹理(SDL_Texture)的功能，
 * 并定义了纹理和字体的智能指针删除器。
 */
#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>

#include "OpenCore/Core/Helpers/Debugger.hpp"

#include <memory>
#include <source_location>
#include <string>

using std::string;
using std::unique_ptr;

/**
 * @struct TextureDeleter
 * @brief SDL_Texture 和 TTF_Font 的智能指针删除器。
 *
 * 用于 unique_ptr 的自定义删除器，确保资源正确释放。
 */
struct TextureDeleter
{
    void operator()(SDL_Texture *texture) const;
    void operator()(TTF_Font *font) const;
};

using TexturePtr = unique_ptr<SDL_Texture, TextureDeleter>;
using FontPtr = unique_ptr<TTF_Font, TextureDeleter>;

/**
 * @brief 从文件加载图像并转换为优化格式的表面。
 *
 * 该函数使用 IMG_Load 加载指定路径的图像，然后将表面转换为 ABGR8888 格式，
 * 以提高后续纹理创建效率。原始表面会被释放，仅返回转换后的表面。
 *
 * @param path 图像文件的路径。
 * @return SDL_Surface* 转换后的表面指针，失败时返回 nullptr。
 *         调用者负责在不再需要时调用 SDL_FreeSurface 释放。
 *
 * @note 若加载或转换失败，会通过 Log 输出错误信息。
 */
inline SDL_Surface *LoadSurface(const string &path)
{
    SDL_Surface *surface = IMG_Load(path.c_str());

    if (!surface)
    {
        LOG("表面加载失败 {}", IMG_GetError());
        return nullptr;
    }
    SDL_Surface *convertedSurface =
        SDL_ConvertSurfaceFormat(surface,
                                 SDL_PIXELFORMAT_ABGR8888, // 或其他需要的格式
                                 0);

    SDL_FreeSurface(surface);

    if (!convertedSurface)
    {
        LOG("SDL出现致命错误 错误代码:{}", SDL_GetError());
        return nullptr;
    }

    LOG("表面已经成功加载到内存 源路径:{}", path.c_str());
    return convertedSurface;
}
/**
 * @brief 将 SDL_Surface 转换为 SDL_Texture。
 *
 * 使用给定的渲染器从表面创建纹理，创建后立即释放表面。
 *
 * @param renderer SDL 渲染器指针，不能为 nullptr。
 * @param surface  要转换的表面指针。函数会接管该表面的所有权并负责释放。
 * @return TexturePtr 包含纹理的智能指针，若 renderer
 * 为空或纹理创建失败则返回空指针。
 *
 * @note 传入的 surface
 * 参数无论转换成功与否都会被释放（成功时内部释放，失败时手动释放）。
 */
inline TexturePtr ConvertSurfaceToTexture(SDL_Renderer *renderer,
                                          SDL_Surface *surface)
{
    if (!renderer)
    {
        LOG("渲染器为空，无法转换表面");
        SDL_FreeSurface(surface);
        return nullptr;
    }

    // 创建纹理
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture)
    {
        LOG("创建纹理时遇到错误 {}", SDL_GetError());
        return nullptr;
    }

    return TexturePtr(texture);
}