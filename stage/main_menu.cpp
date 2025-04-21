#include "main_menu.h"
#include <cmath>

main_stage::main_stage(stageController* sCon)
{
    // 构造函数
    this->sController = sCon;
    this->renderer = sCon->getRenderer(); // 获取渲染器
    this->window = sCon->getWindow(); // 获取窗口
    this->timer = sCon->getTime(); // 获取定时器

    // 初始化资源
    atlas = new Atlas(); // 创建纹理图集

    SDL_Texture* background = IMG_LoadTexture(renderer, RES_MAINMENU_BACKGROUND); // 加载背景图片
    
    atlas->pushElement(new AtlasElement(background)); // 添加背景图片到纹理图集
}

bool main_stage::onUpdate()
{
    return true;
}

bool main_stage::onRender()
{
    int width, height;
    SDL_GetWindowSize(window, &width, &height); // 获取窗口大小

    SDL_Rect dstRect = {width*sin((M_PI/6000)*timer->getTime()),0,width,height};

    SDL_RenderCopy(renderer, atlas->getElement(0)->getTexture(), NULL, &dstRect); // 渲染纹理
    
    dstRect = {width*sin((M_PI/6000)*timer->getTime())-width,0,width,height};
    

    SDL_RenderCopy(renderer, atlas->getElement(0)->getTexture(), NULL, &dstRect); // 渲染纹理
    
    return true;
}

bool main_stage::handleEvent(SDL_Event &event)
{
    if(event.type == SDL_QUIT) // 处理退出事件
    {
        return false;
    }
    return true;
}
