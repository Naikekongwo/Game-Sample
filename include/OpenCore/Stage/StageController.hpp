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

enum index{root,overlay,top};

class StageController {
    public:
    ~StageController() = default;

    // 切换场景，默认为root；
    void changeStage(std::unique_ptr<Stage> newStage);
    //通过id切换场景
    void changeStage(std::unique_ptr<Stage> newStage,uint8_t index);
    //加入临时场景（这里是指比如说鼠标悬停在什么上会弹出一个说明之类的）
    void addTempStage(std::unique_ptr<Stage> newStage);
    //删除前三个场景
    bool deleteStage(uint8_t index);
    //删除最后一个临时场景
    bool deleteTempStage();
    //交换前三个场景,比如拖到物品时其图层可能在菜单图标之上，此时需要暂时交换图层
    void swapStage(uint8_t index1,uint8_t index2);
    // 处理事件的公共接口
    bool handlEvents(SDL_Event* event);

    // 更新逻辑
    void onUpdate();
    
    // 渲染逻辑
    void onRender();
    
    private:
    std::vector <std::unique_ptr<Stage>> currentStages; // 当前场景,大体上是这样的，强三层为固有的（可以为空），后面可以加临时场景
};

#endif //_STAGECONTROLLER_H_