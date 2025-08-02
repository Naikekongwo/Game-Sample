// Sprite.cpp
// 精灵类的实现
#include <SDL2/SDL.h>

#include "OpenCore/ResourceManager.h"
#include "OpenCore/Graphics/Sprite.h"
#include "OpenCore/Animation/FrameAnimation.h"

Sprite::Sprite(short textureID)
{
    texture = ResourceManager::Get().GetTexture(textureID);

    // 激活精灵
    Initialize();
}

void Sprite::Initialize()
{
    anistate = std::make_unique<AnimationState>();
    
}

void Sprite::setTextureScale(int width, int height)
{
    this->width = width;
    this->height = height;
    // 设定单帧的大小
    
    if(texture)
    {
        //如果材质存在
        int w,h;
        SDL_QueryTexture(texture, nullptr, NULL, &w, &h);
        if(!(width == w && height == h)) isMultiFrame = true;
        // 多帧贴图的判断

        xCount = w / width;
        yCount = h / height;
        // 计算动态贴图的布局

    }
}

void Sprite::setAnimation(std::shared_ptr<IAnimation> anim)
{
    // 切换到新的动画
    animation = std::move(anim);
}

void Sprite::onUpdate(float globalTime)
{
    if(animation)
    {
        // 如有动画就直接更新动画
        animation->onUpdate(globalTime, *anistate);
    }
}


void Sprite::onRender(SDL_Renderer* renderer)
{
    Uint8 alpha = static_cast<Uint8>(anistate->transparency * 255.0f);
    SDL_SetTextureAlphaMod(texture, alpha);
    
    if(isMultiFrame)
    {
        int frameIndex = anistate->frameIndex;
        int frameX = frameIndex % xCount;
        int frameY = frameIndex / xCount;

        SDL_Rect srcRect;
        srcRect.x = frameX * width;
        srcRect.y = frameY * height;
        srcRect.w = width;
        srcRect.h = height;
        // 多帧贴图
        SDL_RenderCopy(renderer, texture, &srcRect, &renderRect );
    }
}

void Sprite::setRect(int x, int y, int w, int h)
{
    renderRect.x = x;
    renderRect.y = y;
    renderRect.w = w;
    renderRect.h = h;
    // 设置精灵的渲染范围
}

void Sprite::resetAnime(float totalTime)
{
    animation->reset(totalTime, *anistate);
}