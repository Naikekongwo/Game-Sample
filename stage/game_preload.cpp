#include "game_preload.h"

#include <cmath>

game_preload::game_preload(stageController* sCon)
{
    // 构造函数
    this->sController = sCon;
    this->renderer = sCon->getRenderer(); // 获取渲染器
    this->window = sCon->getWindow(); // 获取窗口
    this->timer = sCon->getTime(); // 获取定时器

    // 初始化资源
    atlas = new Atlas(); // 创建纹理图集

    SDL_Surface* surface = IMG_Load(RES_PRELOAD_CORE_ICON); // 加载背景图片
    SoftInvertColors(surface);
    SDL_Surface* surface_temp = SDL_CreateRGBSurface(0, 500, 500, 32, 0, 0, 0, 0); // 创建新的表面
    SDL_Rect dstRect = {0,0,500,500};
    SDL_BlitScaled(surface, NULL, surface_temp, &dstRect); // 缩放图片

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface_temp);

    atlas->pushElement(new AtlasElement(texture));

    SDL_FreeSurface(surface_temp); // 释放表面
    SDL_FreeSurface(surface); // 释放表面
}


bool game_preload::onUpdate()
{   
    if(timer->getTime() == FRAME_RATE-1)
    {
        sController->changeState(new main_stage(sController)); // 切换到主菜单场景
        return false; // 返回false表示不再更新
    }
    return true;
}

bool game_preload::onRender()
{
    SDL_Rect dstRect;

    SDL_Texture* tn = atlas->getElement(currentIndex)->getTexture();

    AlignRect(tn, dstRect, ALIGN_CENTER | ALIGN_MIDDLE); // 对齐矩形

    SDL_RenderCopy(renderer, tn , NULL, &dstRect); // 渲染纹理

    return true;
}

bool game_preload::handleEvent(SDL_Event &event)
{
    if(event.type == SDL_QUIT) // 处理退出事件
    {
        return false;
    }
    return true;
}


game_preload::~game_preload()
{
    // 析构函数
    delete atlas; // 释放纹理图集
    atlas = nullptr;
}
    