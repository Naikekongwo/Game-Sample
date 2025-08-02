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

    SDL_Rect renderRect;
    GetRect(renderRect);
    
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
    else
    {
        SDL_RenderCopy(renderer, texture, NULL , &renderRect );
    }
}

void Sprite::resetAnime(float totalTime)
{
    animation->reset(totalTime, *anistate);
}

void Sprite::GetRect(SDL_Rect& rect)
{
    const int scaledWidth  = static_cast<int>(anistate->scaleX * width);
    const int scaledHeight = static_cast<int>(anistate->scaleY * height);

    rect.w = scaledWidth;
    rect.h = scaledHeight;

    float offsetX = 0.5f;
    float offsetY = 0.5f;

    switch (anistate->Anchor)
    {
        case 1: offsetX = 0.0f; offsetY = 0.0f; break; // 左上
        case 2: offsetX = 0.5f; offsetY = 0.0f; break; // 中上
        case 3: offsetX = 1.0f; offsetY = 0.0f; break; // 右上
        case 4: offsetX = 0.0f; offsetY = 0.5f; break; // 左中
        case 5: offsetX = 0.5f; offsetY = 0.5f; break; // 中心
        case 6: offsetX = 1.0f; offsetY = 0.5f; break; // 右中
        case 7: offsetX = 0.0f; offsetY = 1.0f; break; // 左下
        case 8: offsetX = 0.5f; offsetY = 1.0f; break; // 中下
        case 9: offsetX = 1.0f; offsetY = 1.0f; break; // 右下
        default: break; // 默认中心对齐
    }

    rect.x = static_cast<int>(anistate->PositionX - offsetX * scaledWidth);
    rect.y = static_cast<int>(anistate->PositionY - offsetY * scaledHeight);
}


void Sprite::setPosition(int x, int y)
{
    anistate->PositionX = x;
    anistate->PositionY = y;
    // 设置坐标的函数
}

void Sprite::setAnchor(int anchor)
{
    // 设定锚点的函数
    if(1<= anchor <=9)
    {
        anistate->Anchor = anchor;
    }
    else
        SDL_Log("Sprite::setAnchor() anchor value is out of range : %d", anchor);
}

void Sprite::setScale(float scaleX, float scaleY)
{
    anistate->scaleX = scaleX;
    anistate->scaleY = scaleY;
}