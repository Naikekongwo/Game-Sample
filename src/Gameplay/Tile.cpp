#include "Eclipsea/Gameplay/Sprite/Tile.h"
#include "OpenCore.hpp"
#include "Runtime/Graphics/IDrawableObject/Texture.hpp"
#include <SDL3/SDL_render.h>
#include <cmath>
#include <memory>

Tile::Tile()
{
    this->id           = "Tile-Regular";
    this->layer        = 0;
    this->VState       = std::make_unique<VisualState>();
    this->AnimeManager = std::make_unique<AnimationManager>();

    texture.reset();
}

void Tile::onEnter()
{
    // 初始化纹理库（直接以资源名同步加载，旧数字 ID 已移除）
    auto *package = OpenEngine::getInstance().getPackageManager();
    TerrainTiles  = std::make_unique<Texture>(
        8, 8, package ? package->getTextureAsync("terrain_terrain") : nullptr);
}

void Tile::Draw()
{
    auto info = OpenEngine::getInstance().getGameInfo();

    Rect dstRect = getLogicalBounds();
    Rect windowRect{0, 0,
                    static_cast<float>(info->_graphicsInfo.resolutionWidth),
                    static_cast<float>(info->_graphicsInfo.resolutionHeight)};

    if (visible(dstRect, windowRect) && VState->getAlpha() > 0.0f)
    {
        // 可见才绘制
        switch (type)
        {
        case TileType::Terrain:
        {
            Rect srcRect = TerrainTiles->getSubRect(TileID);
            TerrainTiles->Draw(&srcRect, &dstRect, 0.0, nullptr);
            break;
        }
        default:
            break;
        }

        // 调试模式：在 Tile 内部绘制白色描边。
        // 逻辑画布（如 4K）经 logical presentation 缩放到物理窗口后，1 逻辑像素
        // 可能 < 1 物理像素，导致 1px 轮廓线变淡/消失；故按物理输出像素对齐坐标、
        // 线宽保证 ≥1 物理像素（窗口缩放或全屏时均清晰）。
        if (m_debugOutline)
        {
            auto *renderer = OpenCoreManagers::GFXManager.getRenderer();
            if (renderer && dstRect.w > 2.0f && dstRect.h > 2.0f)
            {
                // 逻辑画布 → 物理输出像素的缩放比
                int outW = 0, outH = 0;
                SDL_GetRenderOutputSize(renderer, &outW, &outH);
                const float scaleX =
                    (outW > 0)
                        ? static_cast<float>(outW) /
                              static_cast<float>(info->_graphicsInfo
                                                     .resolutionWidth)
                        : 1.0f;
                const float scaleY =
                    (outH > 0)
                        ? static_cast<float>(outH) /
                              static_cast<float>(info->_graphicsInfo
                                                     .resolutionHeight)
                        : 1.0f;

                // 物理 1px 对应的逻辑线宽（保证窗口下至少 1 物理像素可见）
                const float lineW = 1.0f / scaleX;
                const float lineH = 1.0f / scaleY;

                // 坐标对齐到物理像素栅格：逻辑 → 物理(round) → 逻辑
                auto snapX = [scaleX](float lx)
                { return std::round(lx * scaleX) / scaleX; };
                auto snapY = [scaleY](float ly)
                { return std::round(ly * scaleY) / scaleY; };

                // 内缩 1 逻辑像素（保持原设计意图）
                Rect inset = dstRect;
                inset.x += 1.0f;
                inset.y += 1.0f;
                inset.w -= 2.0f;
                inset.h -= 2.0f;

                const float sx = snapX(inset.x);
                const float sy = snapY(inset.y);
                const float ex = snapX(inset.x + inset.w);
                const float ey = snapY(inset.y + inset.h);

                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                // 上 / 下（水平线）
                SDL_FRect top{sx, sy, ex - sx, lineH};
                SDL_FRect bottom{sx, ey - lineH, ex - sx, lineH};
                // 左 / 右（垂直线）
                SDL_FRect left{sx, sy, lineW, ey - sy};
                SDL_FRect right{ex - lineW, sy, lineW, ey - sy};

                SDL_RenderFillRect(renderer, &top);
                SDL_RenderFillRect(renderer, &bottom);
                SDL_RenderFillRect(renderer, &left);
                SDL_RenderFillRect(renderer, &right);
            }
        }
    }
}

bool Tile::onDestroy() { return true; }