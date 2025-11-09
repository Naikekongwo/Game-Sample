// MainStage.hpp
// 定义主页面类的头文件
#ifndef _MAINSTAGE_H_
#define _MAINSTAGE_H_

#include "OpenCore/Stage/StageController.hpp"

class StageController;

class ElementManager;

class MainStage : public Stage
{
    public:
    // 构造函数
    MainStage(Timer* timer, StageController* sController);

    // 完成继承
    bool handlEvents(SDL_Event* event);

    void onUpdate();

    void onRender();

    // 准备函数
    void Init();

    private:

    std::unique_ptr<ElementManager> Elements;
};

#endif //_MAINSTAGE_H_