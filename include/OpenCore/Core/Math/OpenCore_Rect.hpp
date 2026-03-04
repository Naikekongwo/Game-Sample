#pragma once

// OpenCore_Rect
// OpenCore 的点和矩形类（内部 float，输出直接 round）

#include "SDL2/SDL.h"
#include <cmath>

struct OpenCore_Rect
{
    float x;
    float y;
    float w;
    float h;

    OpenCore_Rect() : x(0.0f), y(0.0f), w(0.0f), h(0.0f) {}
    OpenCore_Rect(float _x, float _y, float _w, float _h)
        : x(_x), y(_y), w(_w), h(_h)
    {
    }

    // SDL 支持
    OpenCore_Rect(const SDL_Rect &rect)
    {
        x = static_cast<float>(rect.x);
        y = static_cast<float>(rect.y);
        w = static_cast<float>(rect.w);
        h = static_cast<float>(rect.h);
    }

    operator SDL_Rect() const
    {
        SDL_Rect rect;

        // 用 left/right 差值法避免 1px 漏洞
        int left = static_cast<int>(std::round(x));
        int top = static_cast<int>(std::round(y));
        int right = static_cast<int>(std::round(x + w));
        int bottom = static_cast<int>(std::round(y + h));

        rect.x = left;
        rect.y = top;
        rect.w = right - left;
        rect.h = bottom - top;

        return rect;
    }
};

struct OpenCore_Point
{
    float x;
    float y;

    OpenCore_Point() : x(0.0f), y(0.0f) {}
    OpenCore_Point(float _x, float _y) : x(_x), y(_y) {}

    // SDL 支持
    OpenCore_Point(const SDL_Point &point)
    {
        x = static_cast<float>(point.x);
        y = static_cast<float>(point.y);
    }

    operator SDL_Point() const
    {
        SDL_Point point;
        point.x = static_cast<int>(std::round(x));
        point.y = static_cast<int>(std::round(y));
        return point;
    }
};

using Rect = OpenCore_Rect;
using Point = OpenCore_Point;

// 半开区间（标准图形做法）
inline bool PointInRect(const Point &point, const Rect &rect)
{
    return (point.x >= rect.x && point.x < rect.x + rect.w) &&
           (point.y >= rect.y && point.y < rect.y + rect.h);
}

inline bool visible(const Rect &dstRect, const Rect &window)
{
    return !((dstRect.x >= window.w or dstRect.x + dstRect.w <= 0) or
             (dstRect.y + dstRect.h <= 0 or dstRect.y >= window.h));
}