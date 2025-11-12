#ifndef _STAGECONTROLLER_H_
#define _STAGECONTROLLER_H_

#include "BaseStage.hpp"
#include "OverlayStage.hpp"
#include "Stage.hpp"
#include "TopStage.hpp"

#include <array>
#include <memory>
#include <queue>

using std::unique_ptr;

enum Operation : uint8_t
{
    Add,
    Remove
};

struct OperateRecord
{
    Operation opt;
    StageType sType;
    unique_ptr<Stage> stage_;

    OperateRecord(Operation opt, StageType sType, unique_ptr<Stage> stage_)
        : opt(opt), sType(sType), stage_(std::move(stage_))
    {
    }
    // 初始化函数
};

class StageController
{
  public:
    ~StageController() = default;

    // 通用场景切换（按类型自动分配，延迟执行）
    void changeStage(unique_ptr<Stage> newStage);

    // 销毁某场景
    void removeStage(StageType sType);

    // 处理事件的公共接口
    bool handlEvents(SDL_Event *event);

    // 更新逻辑（在这里应用所有延迟操作）
    void onUpdate();

    // 渲染逻辑
    void onRender();

    // 获取特定类型的场景
    BaseStage *getBaseStage() const
    {
        return dynamic_cast<BaseStage *>(stageContainer[0].get());
    }
    OverlayStage *getOverlayStage() const
    {
        return dynamic_cast<OverlayStage *>(stageContainer[1].get());
    }
    TopStage *getTopStage() const
    {
        return dynamic_cast<TopStage *>(stageContainer[2].get());
    }

    // 检查场景是否存在
    bool hasBaseStage() const { return stageContainer[0] != nullptr; }
    bool hasOverlayStage() const { return stageContainer[1] != nullptr; }
    bool hasTopStage() const { return stageContainer[2] != nullptr; }

  protected:

    void ParsingStreamLine();

  private:
    std::array<unique_ptr<Stage>, 3>
        stageContainer; // [0]:base, [1]:overlay, [2]:top
    std::queue<unique_ptr<OperateRecord>> StreamLine;
};

#endif //_STAGECONTROLLER_H_