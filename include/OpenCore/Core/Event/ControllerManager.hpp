#pragma once
#include <SDL2/SDL.h>
#include <unordered_map>
#include <shared_mutex>

class ControllerManager {
public:
    static ControllerManager& GetInstance();

    void Init();
    void Shutdown();

    // 由主循环调用，处理手柄事件
    void HandleEvent(const SDL_Event& event);

    // 玩家输入查询（玩家索引从 0 开始，若玩家未连接返回默认值）
    bool IsButtonPressed(int playerIndex, SDL_GameControllerButton button) const;
    Uint8 GetButton(int playerIndex, SDL_GameControllerButton button) const;
    Sint16 GetAxis(int playerIndex, SDL_GameControllerAxis axis) const;

    // 获取玩家对应的控制器指针（用于高级操作，如震动）
    SDL_GameController* GetControllerForPlayer(int playerIndex) const;

    // 根据设备实例 ID 获取玩家索引，未找到返回 -1
    int GetPlayerIndexFromInstanceID(SDL_JoystickID instanceId) const;

    // 在主线程同步触发手柄震动（非阻塞）
    void RumblePlayer(int playerIndex, Uint16 lowFreq, Uint16 highFreq, Uint32 durationMs);

    // 获取当前已连接的玩家数量
    size_t GetConnectedPlayerCount() const;

private:
    ControllerManager() = default;
    ~ControllerManager() = default;
    ControllerManager(const ControllerManager&) = delete;
    ControllerManager& operator=(const ControllerManager&) = delete;

    struct ControllerInfo {
        SDL_GameController* controller;
        SDL_JoystickID instanceId;
        Uint8 buttonState[SDL_CONTROLLER_BUTTON_MAX];
        Sint16 axisState[SDL_CONTROLLER_AXIS_MAX];
    };

    mutable std::shared_mutex rw_mutex_;  // 保护 m_players 和 m_instanceToPlayer

    std::unordered_map<int, ControllerInfo> m_players;          // 玩家索引 -> 控制器信息
    std::unordered_map<SDL_JoystickID, int> m_instanceToPlayer; // 实例 ID -> 玩家索引

    int FindFreePlayerIndex() const;  // 需在锁保护下调用
};