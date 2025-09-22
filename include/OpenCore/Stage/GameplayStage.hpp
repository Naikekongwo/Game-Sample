#pragma once
#include "StageController.hpp"

// GameplayStage.hpp
// 游戏场景的头

// 前向声明
class StageController;
class ElementManager;

class GameplayStage : public Stage {
    public:
    // 构造函数
    GameplayStage(Timer* timer, StageController* sController);

    // 完成继承
    bool handlEvents(SDL_Event* event);

    void onUpdate();

    void onRender();

    // 准备函数
    void Init();

    private:

    std::unique_ptr<ElementManager> Elements;
};

