#ifndef _SCALEMANAGER_H_
#define _SCALEMANAGER_H_

// ScaleManager.hpp
// 全局尺寸控制类

#include <SDL2/SDL.h>

class ScaleManager {
public:
    ScaleManager(int baseW, int baseH);

    // 每次窗口大小变化都应调用一次
    void UpdateTargetSize(int windowW, int windowH);

    // 转换逻辑坐标 -> 实际坐标
    SDL_Rect ToScreen(const SDL_Rect& logical) const;
    SDL_FPoint ToScreen(const SDL_FPoint& logical) const;

    // 转换实际坐标 -> 逻辑坐标
    SDL_Point ToLogical(const SDL_Point& screenPoint) const;


    float GetScale() const { return scale; }
    SDL_Point GetOffset() const { return { static_cast<int>(offsetX), static_cast<int>(offsetY) }; }

private:
    int baseWidth, baseHeight;
    int windowWidth, windowHeight;

    float scale = 1.0f;
    float offsetX = 0.0f;
    float offsetY = 0.0f;
};



#endif //_SCALEMANAGER_H_