// Sprite.h
// 精灵类
#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <SDL2/SDL.h>
#include <memory>

struct AnimationState;

class IAnimation; // 前向声明的动画类

class Sprite
{
    public:
    // 构造方法：导入纹理ID
    Sprite(short textureID);

    void Initialize();

    void setAnimation(std::shared_ptr<IAnimation> anim);

    void setTextureScale(int width, int height);
    
    void onUpdate(float globalTime);

    void onRender(SDL_Renderer* renderer);

    void resetAnime(float totalTime);

    void setPosition(int x, int y);

    void setScale(float scaleX, float scaleY);

    void setAnchor(int anchor);

    private:
    // 纹理在资源管理器中的ID
    SDL_Texture* texture;
    // 是否为多帧贴图
    bool isMultiFrame = false;
    // 状态
    std::unique_ptr<AnimationState> anistate;
    // 动画对象的智能指针
    std::shared_ptr<IAnimation> animation;
    // 单帧大小
    int width, height;
    // 动态贴图的布局
    int xCount, yCount;

    void GetRect(SDL_Rect &rect);
};

#endif //_SPRITE_H_