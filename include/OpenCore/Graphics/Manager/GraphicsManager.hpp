#ifndef _GFXCORE_H_
#define _GFXCORE_H_

// GfxManager.hpp
// 图形核心

#include <SDL2/SDL.h>
#include <string>

#include "OpenCore/Graphics/Renderer/IRenderer.hpp"
#include "OpenCore/Graphics/Renderer/SDL_Adapter.hpp"

#include <memory>

// 尺寸类的前向声明
class ScaleManager;

class GraphicsManager
{
  public:
    // 图形管理器目前已经作为单例存在
    static GraphicsManager &getInstance();

    // 初始化函数
    bool Init();

    // 清理回收的函数
    void CleanUp();

    ScaleManager *getScale() const { return this->ContentScale.get(); }

    int RenderCopyEx(SDL_Texture *texture, const OpenCore_Rect *srcrect,
                     const OpenCore_Rect *dstrect, const double angle,
                     const SDL_Point *center, const SDL_RendererFlip flip);
    int RenderCopy(SDL_Texture *texture, const OpenCore_Rect *srcrect,
                   const OpenCore_Rect *dstrect, const double angle,
                   const SDL_Point *center, const SDL_RendererFlip flip);

    int setOffScreenRender(OpenCore_Tilemap *tileMap);

    int RenderTile(OpenCore_Tilemap &Tile, OpenCore_Rect *srcRect, OpenCore_Rect *dstRect, Tilemap_Info Info)
    {
      return Renderer->RenderTilemap(Tile, srcRect, dstRect, Info);
    }

    IRenderer* getRenderEngine();

    OpenCore_Tilemap *createTexture(uint16_t w, uint16_t h);

    void setScale(uint16_t w, uint16_t h);

  private:
    std::unique_ptr<IRenderer> Renderer;
    std::unique_ptr<ScaleManager> ContentScale;
};

#endif //_GFXCORE_H_