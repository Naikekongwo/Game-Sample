#include "OpenCore/OpenCore.h"

ScaleManager::ScaleManager(int baseW, int baseH)
    : baseWidth(baseW), baseHeight(baseH) {}

void ScaleManager::UpdateTargetSize(int windowW, int windowH)
{
    windowWidth = windowW;
    windowHeight = windowH;

    float scaleX = static_cast<float>(windowW) / baseWidth;
    float scaleY = static_cast<float>(windowH) / baseHeight;

    // 保持纵横比一致，取较小比例进行缩放
    scale = (scaleX < scaleY) ? scaleX : scaleY;

    offsetX = (windowW - baseWidth * scale) * 0.5f;
    offsetY = (windowH - baseHeight * scale) * 0.5f;
    SDL_Log("ScaleManager::UpdateTargetSize() Now we are using %d x %d", windowWidth, windowHeight);
}

SDL_Rect ScaleManager::ToScreen(const SDL_Rect &logical) const
{
    SDL_Rect r;
    r.x = static_cast<int>(logical.x * scale + offsetX);
    r.y = static_cast<int>(logical.y * scale + offsetY);
    r.w = static_cast<int>(logical.w * scale);
    r.h = static_cast<int>(logical.h * scale);
    return r;
}

SDL_FPoint ScaleManager::ToScreen(const SDL_FPoint &logical) const
{
    return SDL_FPoint{
        logical.x * scale + offsetX,
        logical.y * scale + offsetY};
}

SDL_Point ScaleManager::ToLogical(const SDL_Point &screenPoint) const
{
    SDL_Point logical;

    // 减去偏移再除以缩放
    logical.x = static_cast<int>((screenPoint.x - offsetX) / scale);
    logical.y = static_cast<int>((screenPoint.y - offsetY) / scale);

    return logical;
}
