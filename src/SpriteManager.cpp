// SpriteManager.cpp
// 负责精灵管理器的实现

#include "OpenCore/OpenCore.h"

void SpriteManager::onUpdate(float globalTime)
{
    for(auto &[id, SpritePtr]: Sprites)
    {
        SpritePtr->onUpdate(globalTime);
    }
}

void SpriteManager::onRender(SDL_Renderer* renderer)
{
    for(auto &[id, SpritePtr] : Sprites)
    {
        SpritePtr->onRender(renderer);
    }
}

Sprite* SpriteManager::getSpriteByID(int id)
{
    auto SpritePtr = Sprites.find(id);

    Sprite* result = (SpritePtr != Sprites.end())?SpritePtr->second.get():nullptr;

    if(result == nullptr)   SDL_Log("SpriteManager::Failed to get sprite id: %d", id);
    // 报错逻辑
    return result;
}


Sprite* SpriteManager::CreateSprite(short spriteID, short textureID)
{
    using namespace std;

    Sprites.emplace(spriteID, make_unique<Sprite>(textureID));
    // 插入新的精灵进入无序图

    return getSpriteByID(spriteID);
    // 此时会激活精灵的构造方法
}