// PreloadStage.cpp
// 预加载页面的实现


#include "Stage/PreloadStage.h"
#include "OpenCore/Macros.h"

PreloadStage::PreloadStage(SDL_Renderer* render, ResourceManager *resMana, SoundEffectManager *sfxMana)
{
    renderer = renderer;
    resourceManager = resMana;
    sfxManager = sfxMana;

    resourceManager->LoadMusicAsync(0, TEST_MUSIC); // 加载背景音乐
}

bool PreloadStage::handlEvents(SDL_Event* event)
{
    return true;
    // 默认成功
}

void PreloadStage::onUpdate()
{
    if(StartUp==0)
    {
        sfxManager->loadBGMR(0);
        sfxManager->setVolume(64);
        sfxManager->playBGM();
        StartUp = 1; // 标记为已播放
    }
}

void PreloadStage::onRender()
{
    
}