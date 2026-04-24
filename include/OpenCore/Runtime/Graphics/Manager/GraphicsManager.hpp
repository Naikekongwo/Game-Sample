/**
 * @file GraphicsManager.hpp
 * @author Naikekongwo
 * @brief
 * @version 0.1
 * @date 2026-03-08
 *
 * @copyright OpenGames Studio Copyright (c) 2026
 *
 */

#ifndef _GFXCORE_H_
#define _GFXCORE_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_log.h>

#include "OpenCore/Core/Math/OpenCore_Rect.hpp"
#include <string>

#include <cstdarg>
#include <memory>

enum class RenderViewport
{
    Fullscreen,
    Left,
    Right,
};

inline void Console_Log(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    // 四个参数都提供
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, fmt,
                    args);

    va_end(args);
}

/**
 * @brief 图形模块
 * @todo 急需加入视窗裁切的相关方法
 */
class GraphicsManager
{
  public:
    /**
     * @brief 获取图形管理器的实例
     *
     * @return GraphicsManager&
     */
    static GraphicsManager &getInstance();

    /**
     * @brief 初始化图形管理器
     *
     * @return true
     * @return false
     */
    bool Init();

    /**
     * @brief 刷新窗口信息
     *
     */
    void refreshWindowProperties();

    /**
     * @brief 清理回收相关数据
     *
     */
    void CleanUp();

    /**
     * @brief 获取SDL的窗口句柄
     *
     * @return SDL_Window*
     */
    SDL_Window *getWindow() const { return window; }

    /**
     * @brief 获取SDL的渲染器句柄
     *
     * @return SDL_Renderer*
     */
    SDL_Renderer *getRenderer() const { return renderer; }

    int setOffScreenRender(SDL_Texture *texture);

    SDL_Texture *createTexture(uint16_t w, uint16_t h);

    Rect getSccissorRect() const
    {
        Rect window{0, 0, 0, 0};
        window.w = TargetWindowWidth;
        window.h = TargetWindowHeight;
        return window;
    }

    /**
     * @brief OpenCore的原生绘制方法
     * @todo 纹理需要进行更原生的封装
     * @param texture 纹理
     * @param srcRect 源纹理的子矩形
     * @param dstRect 将要渲染的目标平面上的选定矩形
     * @param angle 纹理旋转角度
     * @param center 纹理中心
     * @return int 执行结果
     */
    int Draw(SDL_Texture *texture, const Rect *srcRect, const Rect *dstRect,
             const double angle, const Point *center);

    /**
     * @brief OpenCore的原生顶点渲染方法
     * @todo 需要进一步原生封装
     * @param texture 纹理
     * @param vertices 顶点
     * @param num_vertices 顶点个数
     * @param indices 索引
     * @param num_indices 索引个数
     * @return int 执行结果
     */
    int DrawSDLGeometry(SDL_Texture *texture, const SDL_Vertex *vertices,
                        int num_vertices, const int *indices, int num_indices);

  private:
    SDL_Window *window;
    SDL_Renderer *renderer;

    int TargetWindowHeight = 1080;
    int TargetWindowWidth = 1920;

    RenderViewport rView = RenderViewport::Fullscreen;
};

#endif //_GFXCORE_H_