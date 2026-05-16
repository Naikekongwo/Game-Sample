/**
 * @file DualGameplayStage.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2026-04-26
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once

#include "OpenCore/Core/Event/EventManager.hpp"
#include "OpenCore/World/Stage/StageManager.hpp"

/**
 * @brief 双人游戏页面
 *
 */
class DualGameplayStage : public BaseStage
{
  public:
    DualGameplayStage(Timer *timer, StageManager *sController);

    /**
     * @brief 事件处理（OpenCore)
     *
     * @param event
     * @return true
     * @return false
     */
    bool parseEvents(Event *event) override;

    /**
     * @brief 事件处理（SDL_Event）
     * @deprecated 即将废弃，只用于转发
     * @param event
     * @return true
     * @return false
     */
    bool handlEvents(SDL_Event *event) override
    {
        Event newEvent = Event(*event);
        return parseEvents(&newEvent);
    }

    void initializeComponents() override;
    void onUpdate() override;
    void onRender() override;

    void onEnter() override {};

    // 准备函数
    void Init();
};