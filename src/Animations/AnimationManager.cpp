#include "OpenCore/OpenCore.hpp"
#include <algorithm>

Texture::Texture(uint8_t x, uint8_t y, std::shared_ptr<SDL_Texture> tex) : xCount(x), yCount(y), texture(tex)
{
    int W,H;

    if(!texture) SDL_Log("Texture::Texture() encountered empty texture in the initialization.");

    SDL_QueryTexture(texture.get(), NULL, NULL, &W, &H);

    width = static_cast<uint16_t>(W);
    height = static_cast<uint16_t>(H);

    if(xCount == 0 || yCount == 0)
    {
        SDL_Log("Texture::Texture() Invalid xCount or yCount value.");
        return;
    }

    width/=xCount;
    height/=yCount;
}

SDL_Rect Texture::getSrcRect(uint8_t index)
{
    // 构造一个0矩阵的常量，避免重复生成
    static const SDL_Rect emptyRect{0, 0, 0, 0};

    if (index < 0 || index >= Size()) {
        SDL_Log("Texture::getSrcRect() index out of range: %d", index);
        return emptyRect;
    }

    uint8_t col = index % xCount;
    uint8_t row = index / xCount;

    return SDL_Rect{
        col * width,
        row * height,
        width,
        height
    };
}

void AnimationManager::onUpdate(float totalTime, AnimationState& state)
{
    std::vector<size_t> eraseList;

    for (size_t i = 0; i < Animations.size(); ++i)
    {
        auto& anime = Animations[i];
        anime->onUpdate(totalTime, state);

        if (!anime->isLoop() && anime->isFinished())
        {
            eraseList.push_back(i);
        }
        else if (sequential)
        {
            // 顺序模式下，一旦遇到未完成动画就跳出，后续动画不执行
            break;
        }
    }

    // 倒序删除已完成动画，避免下标错乱
    for (auto it = eraseList.rbegin(); it != eraseList.rend(); ++it)
    {
        Animations.erase(Animations.begin() + *it);
    }
}


void AnimationManager::pushAnimation(std::shared_ptr<IAnimation> anime)
{
    Animations.push_back(anime);
}

void AnimationManager::eraseAnimation(std::shared_ptr<IAnimation> anime)
{
    auto it = std::find(Animations.begin(), Animations.end(), anime);
    if (it != Animations.end())
        Animations.erase(it);
}

void AnimationManager::clear()
{
    Animations.clear();
}

void AnimationManager::setSequence(bool isSequential)
{
    this->sequential = isSequential;
}