#ifndef _STAGECONTROLLER_H_
#define _STAGECONTROLLER_H_

#include "Stage.hpp"
#include "BaseStage.hpp"
#include "OverlayStage.hpp"
#include "TopStage.hpp"

#include <memory>
#include <array>

class StageController {
public:
    ~StageController() = default;

    // 类型安全的场景切换方法
    void changeBaseStage(std::unique_ptr<BaseStage> newStage);
    void changeOverlayStage(std::unique_ptr<OverlayStage> newStage);
    void changeTopStage(std::unique_ptr<TopStage> newStage);
    
    // 通用场景切换（按类型自动分配）
    void changeStage(std::unique_ptr<Stage> newStage);
    
    // 删除场景
    void deleteBaseStage() { stageContainer[0].reset(); }
    void deleteOverlayStage() { stageContainer[1].reset(); }
    void deleteTopStage() { stageContainer[2].reset(); }
    
    // 处理事件的公共接口
    bool handlEvents(SDL_Event* event);

    // 更新逻辑
    void onUpdate();
    
    // 渲染逻辑
    void onRender();
    
    // 获取特定类型的场景
    BaseStage* getBaseStage() const { 
        return dynamic_cast<BaseStage*>(stageContainer[0].get()); 
    }
    OverlayStage* getOverlayStage() const { 
        return dynamic_cast<OverlayStage*>(stageContainer[1].get()); 
    }
    TopStage* getTopStage() const { 
        return dynamic_cast<TopStage*>(stageContainer[2].get()); 
    }
    
    // 检查场景是否存在
    bool hasBaseStage() const { return stageContainer[0] != nullptr; }
    bool hasOverlayStage() const { return stageContainer[1] != nullptr; }
    bool hasTopStage() const { return stageContainer[2] != nullptr; }
    
private:
    std::array<std::unique_ptr<Stage>, 3> stageContainer; // [0]:base, [1]:overlay, [2]:top
};

#endif //_STAGECONTROLLER_H_