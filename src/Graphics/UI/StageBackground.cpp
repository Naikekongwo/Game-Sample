#include "OpenCore/OpenCore.hpp"

StageBackground::StageBackground(std::string id, short layer, Texture *texture)
{
    AnimeState = std::make_unique<AnimationState>();

    this->id = id;
    this->layer = layer;

    // 创建动画管理器

    AnimeManager = std::make_unique<AnimationManager>();

    // 获取材质
    if (!texture)
    {
        SDL_Log("StageBackground::StageBackground() encountered a empty texture.");
        return;
        // 如果材质为空，那么我们直接强制返回
    }

    this->texture = texture;
}

void StageBackground::setNativeScale(int scale)
{
    nativeScale = scale;
}

void StageBackground::handlEvents(SDL_Event &event, float totalTime)
{
}

int StageBackground::setBakedTexture(bool isBaked)
{
    bakedTexture = isBaked;
    preRenderTexture(nullptr);

    return 0;
}

bool StageBackground::preRenderTexture(SDL_Texture *target)
{
    auto &GFX = GraphicsManager::getInstance();
    SDL_Renderer *renderer = GFX.getRenderer();

    // 设置渲染目标
    if (target)
    {
        SDL_SetRenderTarget(renderer, target);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
    }

    SDL_Rect bounds = getBounds();
    SDL_Rect srcRect{};
    SDL_Rect dstRect{};

    // 离屏时坐标系从 (0,0)
    int baseX = (target ? 0 : bounds.x);
    int baseY = (target ? 0 : bounds.y);

    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
        {
            srcRect = texture->getSrcRect(row * 3 + col);

            // X 方向（列）
            if (col == 0)
            {
                dstRect.x = baseX;
                dstRect.w = nativeScale;
            }
            else if (col == 1)
            {
                dstRect.x = baseX + nativeScale;
                dstRect.w = bounds.w - 2 * nativeScale;
            }
            else
            {
                dstRect.x = baseX + bounds.w - nativeScale;
                dstRect.w = nativeScale;
            }

            // Y 方向（行）
            if (row == 0)
            {
                dstRect.y = baseY;
                dstRect.h = nativeScale;
            }
            else if (row == 1)
            {
                dstRect.y = baseY + nativeScale;
                dstRect.h = bounds.h - 2 * nativeScale;
            }
            else
            {
                dstRect.y = baseY + bounds.h - nativeScale;
                dstRect.h = nativeScale;
            }

            GFX.RenderCopyEx(texture->texture.get(), &srcRect, &dstRect, 0.0, nullptr, SDL_FLIP_NONE);
        }
    }

    if (target)
        SDL_SetRenderTarget(renderer, nullptr);

    return true;
}

void StageBackground::onRender()
{
    auto &GFX = GraphicsManager::getInstance();
    SDL_Rect bounds = getBounds();

    if (bakedTexture)
    {
        if (!baked_Texture)
        {
            // 创建并烘焙
            baked_Texture = std::make_unique<Texture>(
                1, 1, std::shared_ptr<SDL_Texture>(GFX.createTexture(bounds.w, bounds.h), [](SDL_Texture *t)
                                                   { SDL_DestroyTexture(t); } // 自定义删除器
                                                   ));

            preRenderTexture(baked_Texture->texture.get());
        }

        // 每帧只绘制烘焙结果
        GFX.RenderCopyEx(baked_Texture->texture.get(), nullptr, &bounds, 0.0, nullptr, SDL_FLIP_NONE);
    }
    else
    {
        // 即时绘制
        preRenderTexture(nullptr);
    }
}

void StageBackground::onUpdate(float totalTime)
{
}