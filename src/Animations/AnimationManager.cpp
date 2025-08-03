#include "OpenCore/OpenCore.h"

Texture::Texture(int x, int y, SDL_Texture* tex)
{
    this->xCount = x;
    this->yCount = y;

    if(!tex) SDL_Log("Texture::Texture() encountered empty texture in the initialization.");

    this->texture = tex;

    SDL_QueryTexture(texture, NULL, NULL, &width, &height);

    width/=xCount;
    height/=yCount;
}

SDL_Rect Texture::getSrcRect(int index)
{
    if (index < 0 || index >= Size()) {
        SDL_Log("Texture::getSrcRect() index out of range: %d", index);
        return SDL_Rect{0, 0, 0, 0};
    }

    int col = index % xCount;
    int row = index / xCount;

    return SDL_Rect{
        col * width,
        row * height,
        width,
        height
    };
}

void AnimationManager::onUpdate(float totalTime, AnimationState& state)
{
    for(auto &[id, anime] : Animations)
    {
        // 遍历动画表
        anime->onUpdate(totalTime, state);
        // 不循环且结束那就销毁
        if(anime->isFinished() && !anime->isLoop())
        {
            Animations.erase(id);
            continue;
        }
    }
}

void AnimationManager::pushAnimation(int id, std::shared_ptr<IAnimation> anime)
{
    Animations.emplace(id, anime);
    // 加入表中
}

void AnimationManager::eraseAnimation(int id)
{
    auto it = Animations.find(id);
    if(it != Animations.end())
        Animations.erase(id);
}