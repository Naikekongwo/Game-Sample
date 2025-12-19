#ifndef _SDL_ADAPTER_HPP_
#define _SDL_ADAPTER_HPP_

#include <memory>
#include <unordered_map>
#include <vector>
#include <mutex>

#include "IRenderer.hpp"
#include "OpenCore/Graphics/Renderer/IRenderer.hpp"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

using std::pair;
using std::shared_ptr;
using std::vector;

class SDL_Adapter;

class S_TEXTURE : public OpenCore_Renderer::OpenCore_Tilemap
{
    friend SDL_Adapter;

  public:
    S_TEXTURE(SDL_Texture *texture);

    bool QueryTile(int &w, int &h) override;

    int Destroy() override;

  private:
    SDL_Texture *getTexture();
    SDL_Texture *sTexture;
};

class SDL_Adapter : public IRenderer
{
  public:
    bool InitRenderer(Window_Info &Info) override;

    int RenderTilemap(OpenCore_Tilemap &Tile, OpenCore_Rect *srcRect,
                      OpenCore_Rect *dstRect, Tilemap_Info Info) override;

    void RendererCleanUp() override;

    OpenCore_Tilemap *createTexture(uint16_t w, uint16_t h) override;

    int setOffScreenRenderer(OpenCore_Tilemap *tileMap) override;

    void LoadTile(short id, const string &path) override;

    void SyncFunction() override;

    // 临时充数的函数！

    SDL_Renderer *getRenderer() const { return renderer; }
    SDL_Window *getWindow() const { return window; }

    int RenderVertices(OpenCore_Tilemap *tilemap, const SDL_Vertex *vertices,
                       int num_vertices, const int *indices, int num_indices);

  protected:
    SDL_Texture getTexture(OpenCore_Tilemap &tileMap);

    SDL_Texture *transmitSurface(SDL_Surface *surface, short id);

    SDL_Surface *LoadSurface(const string &path);

  private:
    SDL_Window *window;
    SDL_Renderer *renderer;

    vector<pair<short, SDL_Surface *>> rawSurfaces;

    std::mutex rawSurfacesMutex_;  // 新增互斥锁
};

#endif //_SDL_ADAPTER_HPP_