#include "OpenCore/OpenCore.hpp"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <cstddef>
#include <memory>
#include <utility>

S_TEXTURE::S_TEXTURE(SDL_Texture *texture)
{
    sTexture = texture;
    this->rType = rendererType::SDL;
}

int S_TEXTURE::Destroy()
{
    SDL_DestroyTexture(sTexture);
    return 0;
}

SDL_Texture *S_TEXTURE::getTexture() { return sTexture; }

bool S_TEXTURE::QueryTile(int &w, int &h)
{
    return (bool)SDL_QueryTexture(sTexture, NULL, NULL, &w, &h);
}

int SDL_Adapter::RenderVertices(OpenCore_Tilemap *tilemap,
                                const SDL_Vertex *vertices, int num_vertices,
                                const int *indices, int num_indices)
{
    if (tilemap->rType != rendererType::SDL)
    {
        SDL_Log("SDL_RENDER Encountered a tilemap not for SDL?");
        return -1;
    }

    S_TEXTURE *sTex = static_cast<S_TEXTURE *>(tilemap);

    SDL_Texture *tex = sTex->getTexture();

    return SDL_RenderGeometry(renderer, tex, vertices, num_vertices, indices,
                              num_indices);
}

bool SDL_Adapter::InitRenderer(Window_Info &Info)
{
    rType = rendererType::SDL;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        SDL_Log("Failed to init SDL as the primary renderer.");
        return false;
    }

    if (TTF_Init() == -1)
    {
        SDL_Log("Failed to init SDL_ttf extension.");
        return false;
    }

    if(IMG_Init(IMG_INIT_PNG) == 0)
    {
        SDL_Log("Failed to init SDL_IMG extension.");
        return false;
    }

    SDL_CreateWindowAndRenderer(Info.WindowGeometry[0], Info.WindowGeometry[1],
                                SDL_WINDOW_SHOWN, &window, &renderer);

    if (!window || !renderer)
    {
        SDL_Log("Failed to create window or renderer.(SDL)");
        return false;
    }

    // 初始化窗口和渲染器完毕

    // 将缩放的品质调为0，这样可以获得最锐利的缩放
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    SDL_SetWindowTitle(window, Info.WindowTitle.c_str());
    SDL_SetWindowResizable(window,
                           (Info.Window_Resizable) ? SDL_TRUE : SDL_FALSE);


    SDL_Log("Successfully!.");
    return true;
}

void SDL_Adapter::RendererCleanUp()
{
    if (!renderer)
        SDL_Log("GFXManager::CleanUp() failed to destroy a null renderer.");
    else
        SDL_DestroyRenderer(renderer);

    if (!window)
        SDL_Log("GFXManager::CleanUp() failed to destroy a null window.");
    else
        SDL_DestroyWindow(window);

    // ContentScale.reset();

    SDL_Log("GFXManager::CleanUp() has reseted the scale system.");

    SDL_Log("GFXManager::CleanUp() manage to quit the SDL.");
    SDL_Quit();
}

int SDL_Adapter::RenderTilemap(OpenCore_Tilemap &Tile, OpenCore_Rect *srcRect,
                               OpenCore_Rect *dstRect, Tilemap_Info Info)
{
    SDL_Rect src = {srcRect->x, srcRect->y, srcRect->w, srcRect->h};
    SDL_Rect dst = {dstRect->x, dstRect->y, dstRect->w, dstRect->h};

    if (Tile.rType != rendererType::SDL)
        return -1;

    SDL_Point center{Info.center->x, Info.center->y};

    S_TEXTURE *sTexture = static_cast<S_TEXTURE *>(&Tile);

    return SDL_RenderCopyEx(renderer, sTexture->getTexture(), &src, &dst,
                            Info.angle, &center, SDL_FLIP_NONE);
}

OpenCore_Tilemap *SDL_Adapter::createTexture(uint16_t w, uint16_t h)
{
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                             SDL_TEXTUREACCESS_TARGET, w, h);
    // 启用透明混合
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    S_TEXTURE *newTile = new S_TEXTURE(texture);

    return newTile;
}

int SDL_Adapter::setOffScreenRenderer(OpenCore_Tilemap *tileMap)
{
    if (tileMap->rType != rendererType::SDL)
        return -1;

    S_TEXTURE *sTexture = static_cast<S_TEXTURE *>(tileMap);

    if (SDL_SetRenderTarget(renderer, sTexture->getTexture()) != 0)
    {
        SDL_Log("Failed to set render target: %s", SDL_GetError());
        return -1;
    }

    if (sTexture->getTexture())
    {
        // 可选：只在需要清理时才清理
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
    }

    return 0;
}

void SDL_Adapter::SyncFunction()
{
    if (!rawSurfaces.empty())
    {
        SDL_Log("Start to transform surface to texture.");
        // 开始转换并返回
        for (auto &entry : rawSurfaces)
        {
            SDL_Texture *newTexture =
                transmitSurface(entry.second, entry.first);

            S_TEXTURE *texture = new S_TEXTURE(newTexture);

            TexturePtr texPtr(static_cast<OpenCore_Tilemap *>(texture));

            OpenCoreManagers::ResManager.PushTexture(entry.first, std::move(texPtr));
            
            SDL_FreeSurface(entry.second);
        }
    }
    rawSurfaces.clear();
}

SDL_Surface *SDL_Adapter::LoadSurface(const string &path)
{
    SDL_Surface *surface = IMG_Load(path.c_str());

    if (!surface)
    {
        std::printf("IMG_Load failed: %s", IMG_GetError());
        return nullptr;
    }
    SDL_Surface *convertedSurface =
        SDL_ConvertSurfaceFormat(surface,
                                 SDL_PIXELFORMAT_ABGR8888, // 或其他需要的格式
                                 0);

    SDL_FreeSurface(surface);

    if (!convertedSurface)
    {
        std::printf("SDL_ConvertSurfaceFormat failed: %s", SDL_GetError());
        return nullptr;
    }

    std::printf(
        "ResourceManager::LoadSurface() %s surface loaded successfully.",
        path.c_str());
    return convertedSurface;
}

SDL_Texture *SDL_Adapter::transmitSurface(SDL_Surface *surface, short id)
{
    if (!renderer)
    {
        std::printf(
            "ResourceManager::ConvertSurfaceToTexture: renderer is null");
        SDL_FreeSurface(surface);
        return nullptr;
    }

    // 创建纹理
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    if (!texture)
    {
        std::printf("SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
        return nullptr;
    }

    return texture;
}

void SDL_Adapter::LoadTile(short id, const string &path)
{
    SDL_Surface *surface = LoadSurface(path);
    if (!surface) {
        std::printf("❌ LoadTile: Failed to load surface for id=%d, path=%s\n", 
                   id, path.c_str());
        return;
    }
    
    // 🔥 添加锁保护
    {
        std::lock_guard<std::mutex> lock(rawSurfacesMutex_);
        rawSurfaces.push_back(std::make_pair(id, surface));
        std::printf("✅ LoadTile: Added surface id=%d, queue size=%zu\n", 
                   id, rawSurfaces.size());
    }
}