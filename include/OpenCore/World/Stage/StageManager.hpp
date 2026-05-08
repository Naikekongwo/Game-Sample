#ifndef _STAGECONTROLLER_H_
#define _STAGECONTROLLER_H_

#include "BaseStage.hpp"
#include "OverlayStage.hpp"
#include "Stage.hpp"
#include "TopStage.hpp"

#include "ScriptStage.hpp"

#include <array>
#include <memory>
#include <queue>

using std::unique_ptr;

class Event;

enum StageCommandType : uint8_t
{
    Add,
    Remove
};

struct StageCommand
{
    StageCommandType opt;
    StageType sType;
    unique_ptr<Stage> stage_;

    StageCommand(StageCommandType opt, StageType sType,
                 unique_ptr<Stage> stage_)
        : opt(opt), sType(sType), stage_(std::move(stage_))
    {
    }
    // 初始化函数
};

class StageManager
{
  public:
    ~StageManager() = default;

    // 通用场景切换（按类型自动分配，延迟执行）
    void changeStage(unique_ptr<Stage> newStage);

    // 销毁某场景
    void removeStage(StageType sType);

    /**
     * @brief 处理事件的公共接口
     * @deprecated 旧的SDL_Event
     * @todo 该接口依赖于旧的SDL_Event，应当删除
     * @param event
     * @return true
     * @return false
     */
    bool handlEvents(SDL_Event *event);

    /**
     * @brief OpenCore的事件处理方法
     *
     * @param event
     * @return true
     * @return false
     */
    bool parseEvent(Event *event);

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
    void processCommandQueue();

  private:
    //   定义场景池的大小为3
    static constexpr size_t kStageCount = 3;

    std::array<unique_ptr<Stage>, kStageCount> stageContainer;
    // [0]:base, [1]:overlay, [2]:top
    std::queue<unique_ptr<StageCommand>> commandQueue;
};

#endif //_STAGECONTROLLER_H_