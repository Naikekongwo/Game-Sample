#ifndef _STAGECONTROLLER_H_
#define _STAGECONTROLLER_H_

// StageController.hpp
// 场景控制器的头
#include "Stage.hpp"
#include "PreloadStage.hpp"
#include "MainStage.hpp"
#include "GameplayStage.hpp"

#include <memory>
#include <iostream>
#include <vector>
#include <unordered_map>

class StageController {
    public:
    ~StageController() = default;

    // 切换场景
    short changeStage(std::unique_ptr<Stage> newStage);

    //通过id切换场景
    bool changeStage(short id);

    //在场景池中加入场景
    short addStageToCache(std::unique_ptr<Stage> newStage,short id);

    //删除对应id的场景
    bool deleteStage(short id);

    // 处理事件的公共接口
    bool handlEvents(SDL_Event* event);

    // 更新逻辑
    void onUpdate();
    
    // 渲染逻辑
    void onRender();
    
    private:
    std::unique_ptr<Stage> currentStage; // 当前场景
    short currentId;
    std::unordered_map<short,std::unique_ptr<Stage>> stageCache_;//场景池,且场景池中short不应为0和-1，0、-1用于做信号量
};

#endif //_STAGECONTROLLER_H_