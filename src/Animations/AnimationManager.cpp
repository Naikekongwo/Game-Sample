#include "OpenCore/OpenCore.h"

Texture::Texture(int x, int y, SDL_Texture* tex) : xCount(x), yCount(y), texture(tex)
{
    if(!texture) SDL_Log("Texture::Texture() encountered empty texture in the initialization.");

    SDL_QueryTexture(texture, NULL, NULL, &width, &height);

    if(xCount == 0 || yCount == 0)
    {
        SDL_Log("Texture::Texture() Invalid xCount or yCount value.");
        return;
    }

    width/=xCount;
    height/=yCount;
}

SDL_Rect Texture::getSrcRect(int index)
{
    // 构造一个0矩阵的常量，避免重复生成
    static const SDL_Rect emptyRect{0, 0, 0, 0};

    if (index < 0 || index >= Size()) {
        SDL_Log("Texture::getSrcRect() index out of range: %d", index);
        return emptyRect;
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
    std::vector<short> eraseList;

    for( auto &[id, anime] : Animations)
    {
        anime->onUpdate(totalTime, state);

        if(!anime->isLoop() && anime->isFinished())
        {
            eraseList.push_back(id);
        }
    }

    for( auto id : eraseList)
    {
        Animations.erase(id);
    }
}

void AnimationManager::pushAnimation(int id, std::shared_ptr<IAnimation> anime)
{
    // 尝试加入表中
    Animations.try_emplace(id, anime);
}

void AnimationManager::eraseAnimation(int id)
{
    auto it = Animations.find(id);
    if(it != Animations.end())
        Animations.erase(id);
}


void AnimationManager::clear()
{
    Animations.clear();
}