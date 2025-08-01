#ifndef _PRELOADSTAGE_H_
#define _PRELOADSTAGE_H_

// PreloadStage.h
// 预加载页面

#include "StageController.h"

struct AnimationState;
struct TextureLoadTask;
class FrameAnimation;

class PreloadStage : public Stage
{
public:
    PreloadStage(SDL_Renderer* render, ResourceManager *resMana, SoundEffectManager *sfxMana, Timer* timer);

    void LoadResources();

    bool handlEvents(SDL_Event *event) override;

    void onUpdate() override;

    void onRender() override;

    // 以上则为事件处理、事件更新、渲染的逻辑重写

private:
    int stageState = 0;
    // 0为未播放

    std::vector<TextureLoadTask> textureTasks;
    
    std::unique_ptr<AnimationState> anistate;
    std::unique_ptr<FrameAnimation> headAnimation;
};

#endif //_PRELOADSTAGE_H_