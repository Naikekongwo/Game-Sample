// PreloadStage.hpp
// 预加载页面

#ifndef _PRELOADSTAGE_H_
#define _PRELOADSTAGE_H_

#include "OpenCore/Stage/StageController.hpp"

#include <future>

class StageController;
class ElementManager;

class PreloadStage : public BaseStage
{
public:
    PreloadStage(Timer* timer, StageController* sController);

    void LoadResources();

    bool handlEvents(SDL_Event *event) override;

    void onUpdate() override;

    void onRender() override;

    // 以上则为事件处理、事件更新、渲染的逻辑重写

private:
    int stageState = 0;
    // 0为未播放
    std::future<void> LoadingState;
    
};

#endif //_PRELOADSTAGE_H_