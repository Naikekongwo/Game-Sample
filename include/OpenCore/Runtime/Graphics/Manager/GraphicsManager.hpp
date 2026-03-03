#ifndef _GFXCORE_H_
#define _GFXCORE_H_

// GfxManager.hpp
// 图形核心

#include <SDL2/SDL.h>
#include <SDL2/SDL_log.h>

#include "OpenCore/Core/Math/OpenCore_Rect.hpp"
#include <string>

#include <cstdarg>
#include <memory>

inline void Console_Log(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    // 四个参数都提供
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, fmt,
                    args);

    va_end(args);
}

class GraphicsManager
{
  public:
    // 图形管理器目前已经作为单例存在
    static GraphicsManager &getInstance();

    // 初始化函数
    bool Init();

    // 重置窗口详情
    void refreshWindowProperties();

    // 清理回收的函数
    void CleanUp();

    SDL_Window *getWindow() const { return window; }
    SDL_Renderer *getRenderer() const { return renderer; }

    int RenderCopyEx(SDL_Texture *texture, const SDL_Rect *srcrect,
                     const SDL_Rect *dstrect, const double angle,
                     const SDL_Point *center, const SDL_RendererFlip flip);
    int RenderCopy(SDL_Texture *texture, const SDL_Rect *srcrect,
                   const SDL_Rect *dstrect, const double angle,
                   const SDL_Point *center, const SDL_RendererFlip flip);

    int Draw(SDL_Texture *texture, const Rect *srcRect, const Rect *dstRect,
             const double angle, const Point *center);

    int setOffScreenRender(SDL_Texture *texture);

    SDL_Texture *createTexture(uint16_t w, uint16_t h);

  private:
    SDL_Window *window;
    SDL_Renderer *renderer;
};

#endif //_GFXCORE_H_