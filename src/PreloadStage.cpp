// PreloadStage.cpp
// 预加载页面的实现


#include "Stage/PreloadStage.h"

bool PreloadStage::handlEvents(SDL_Event* event)
{
    return true;
    // 默认成功
}

void PreloadStage::onUpdate()
{
    if(StartUp==0)
    {
        musicd.loadBGM("assets/audio/test.mp3");
        musicd.setVolume(64);
        musicd.playBGM();
        StartUp = 1; // 标记为已播放
    }
}

void PreloadStage::onRender()
{
    
}