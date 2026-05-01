/**
 * @file EventManager.hpp
 * @author your name (you@domain.com)
 * @brief 事件管理器
 * @version 0.1
 * @date 2026-04-23
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include <SDL2/SDL.h>


enum class InputMode {
    KeyboardMouse,
    Gamepad
};
class Event
{
  public:
    Event() = default;
    explicit Event(const SDL_Event &sdlEvent) : m_sdlEvent(sdlEvent) {}

    // 隐式转换为 SDL_Event（方便在需要 SDL_Event 的地方直接使用 Event 对象）
    operator SDL_Event() const { return m_sdlEvent; }

    // 获取事件类型（封装 SDL 事件类型）
    Uint32 GetType() const { return m_sdlEvent.type; }

    // 提供对原始 SDL_Event 的访问（当需要指针或修改时使用）
    const SDL_Event &GetSDLEvent() const { return m_sdlEvent; }
    SDL_Event &GetSDLEvent() { return m_sdlEvent; }

  private:
    SDL_Event m_sdlEvent;
};

// 事件管理器（单例模式）
class EventManager
{
  public:
    // 获取唯一实例
    static EventManager &GetInstance();

    // 初始化（如果需要在管理器内额外设置，可在此进行）
    bool Init();
    void Shutdown();

    // 轮询事件：成功返回 true 并填充 event，否则返回 false
    bool PollEvent(Event &event);

    // 等待事件：阻塞直到有事件，成功返回 true 并填充 event
    bool WaitEvent(Event &event);

    // 推送自定义事件到 SDL 事件队列
    void PushEvent(const Event &event);

    // 查询当前输入模式
    InputMode GetInputMode() const;

    // 允许外部强制设置
    void SetInputMode(InputMode mode);

    bool GetAllowtoDivertMod() const;

    void SetAllowtoDivertMod( bool allowtoDivertMod);

    // 在事件循环结束后调用，根据本帧标志更新 m_inputMode
    void EndFrame();




  private:
    EventManager() = default;
    ~EventManager() = default;
    EventManager(const EventManager &) = delete;
    EventManager &operator=(const EventManager &) = delete;
    // 根据事件类型更新 m_inputMode
    void UpdateInputMode(const Event& event);
    InputMode m_inputMode = InputMode::KeyboardMouse;  // 默认键鼠模式
    int allowtoDivertMod = 0; // 允许切换输入模式(默认不允许，1为允许)
    // 帧标志：本帧是否有对应设备事件
    bool m_hasKMEvent = false;   // Keyboard/Mouse
    bool m_hasGamepadEvent = false;

};
