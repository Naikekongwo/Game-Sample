// SpriteManager.h
// 管理精灵类的管理器

#ifndef _SPRITEMANAGER_H_
#define _SPRITEMANAGER_H_

#include "Sprite.h"

#include <memory>
#include <unordered_map>

class SpriteManager
{
    public:

    // 更新函数，用以更新子精灵
    void onUpdate(float globalTime);

    // 渲染函数，用以渲染精灵
    void onRender(SDL_Renderer* renderer);

    // 查找精灵
    Sprite* getSpriteByID(int id);

    // 创建精灵
    Sprite* CreateSprite(short spriteID, short textureID);
    

    private:
    // 用以保存精灵的私有表
    std::unordered_map<int,std::unique_ptr<Sprite>> Sprites;
};

#endif //_SPRITEMANAGER_H_