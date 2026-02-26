// PreloadStage.hpp
// 预加载页面

#ifndef _PRELOADSTAGE_H_
#define _PRELOADSTAGE_H_

#include "OpenCore/Stage/StageManager.hpp"

#include <future>

enum class PreloadPhase
{
    // 预加载阶段的枚举类
    WaitingForResource,
    BuildLoadingUI,
    WaitAnimation,
    InitAudio,
    ShowTitle1,
    ShowTitle2,
    ShowConnector,
    ConnectorOn,
    Finished
};

class StageManager;
class ElementManager;

class PreloadStage : public Stage
{
  public:
    PreloadStage(Timer *timer, StageManager *sController);

    // 生命周期
    void onEnter() override;
    void onExit() override;
    void onDestroy() override;

    // 基类接口
    bool handlEvents(SDL_Event *event) override;
    void onUpdate() override;
    void onRender() override;

  protected:
    // 阶段状态机
    PreloadPhase phase;
    bool pendingNextStage = false;

    // 异步资源加载
    std::future<void> LoadingState;

    // 内部辅助函数
    void buildLoadingUI();
    void buildTitleAndWater();
    void handleTitleSequence();

    // 构建元素引用
    Timer *timer;
    StageManager *sController;
};

#endif //_PRELOADSTAGE_H_