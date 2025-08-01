#ifndef _PRELOADSTAGE_H_
#define _PRELOADSTAGE_H_

// PreloadStage.h
// 预加载页面

#include "StageController.h"
#include "OpenCore/SoundEffect.h"

#include <future>

class PreloadStage : public Stage
{
public:
    PreloadStage(SDL_Renderer* render, ResourceManager *resMana, SoundEffectManager *sfxMana);

    bool handlEvents(SDL_Event *event) override;

    void onUpdate() override;

    void onRender() override;

    // 以上则为事件处理、事件更新、渲染的逻辑重写

private:
    int StartUp = 0;
    // 0为未播放

    std::future<void> texture1;
};

#endif //_PRELOADSTAGE_H_