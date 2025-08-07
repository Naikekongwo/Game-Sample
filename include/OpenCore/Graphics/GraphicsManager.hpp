#ifndef _GFXCORE_H_
#define _GFXCORE_H_

// GfxCore.hpp
// 图形核心

#include <string>
#include <SDL2/SDL.h>

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

    SDL_Window *getWindow() const { return window; }
    SDL_Renderer *getRenderer() const { return renderer; }

    int RenderCopyEx(SDL_Texture *texture,
                     const SDL_Rect *srcrect,
                     const SDL_Rect *dstrect,
                     const double angle,
                     const SDL_Point *center,
                     const SDL_RendererFlip flip);

    void setScale(int w, int h);

private:
    SDL_Window *window;
    SDL_Renderer *renderer;

    std::unique_ptr<ScaleManager> ContentScale;
};

#endif //_GFXCORE_H_