// PreloadStage.cpp
// 预加载页面的实现

#include "OpenCore/OpenCore.h"

PreloadStage::PreloadStage(SDL_Renderer* render, ResourceManager *resMana, SoundEffectManager *sfxMana)
{
    renderer = render;
    resourceManager = resMana;
    sfxManager = sfxMana;

    resourceManager->LoadMusicAsync(0, TEST_MUSIC); // 加载背景音乐

    texture1 = resourceManager->LoadTextureAsync(0, RES_GAME_ICON); // 异步加载图标
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
    if(texture1.valid() && texture1.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
        SDL_Log("加载成功。");
        SDL_Texture* iconTexture = resourceManager->GetTexture(0);
        if (iconTexture) {
            SDL_Log("获取成功。");
            SDL_RenderCopy(renderer, iconTexture, NULL, NULL);
        }
    }
}