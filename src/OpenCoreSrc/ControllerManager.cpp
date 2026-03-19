#include "OpenCore/Core/Event/ControllerManager.hpp"
#include <mutex>          
#include <shared_mutex>   
#include <cstring>        
#include <algorithm>

ControllerManager& ControllerManager::GetInstance() {
    static ControllerManager instance;
    return instance;
}

void ControllerManager::Init() {
    std::unique_lock<std::shared_mutex> lock(rw_mutex_);
    int numJoysticks = SDL_NumJoysticks();
    for (int i = 0; i < numJoysticks; ++i) {
        if (SDL_IsGameController(i)) {
            SDL_GameController* controller = SDL_GameControllerOpen(i);
            if (controller) {
                SDL_JoystickID id = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller));
                int playerIndex = FindFreePlayerIndex();
                ControllerInfo info;
                info.controller = controller;
                info.instanceId = id;
                std::memset(info.buttonState, 0, sizeof(info.buttonState));
                std::memset(info.axisState, 0, sizeof(info.axisState));
                m_players[playerIndex] = info;
                m_instanceToPlayer[id] = playerIndex;
            }
        }
    }
}

void ControllerManager::Shutdown() {
    std::unique_lock<std::shared_mutex> lock(rw_mutex_);
    for (auto& pair : m_players) {
        if (pair.second.controller) {
            SDL_GameControllerClose(pair.second.controller);
        }
    }
    m_players.clear();
    m_instanceToPlayer.clear();
}

void ControllerManager::HandleEvent(const SDL_Event& event) {
    std::unique_lock<std::shared_mutex> lock(rw_mutex_);

    switch (event.type) {
        case SDL_CONTROLLERDEVICEADDED: {
            int deviceIndex = event.cdevice.which;
            SDL_GameController* controller = SDL_GameControllerOpen(deviceIndex);
            if (controller) {
                SDL_JoystickID id = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller));
                int playerIndex = FindFreePlayerIndex();
                ControllerInfo info;
                info.controller = controller;
                info.instanceId = id;
                std::memset(info.buttonState, 0, sizeof(info.buttonState));
                std::memset(info.axisState, 0, sizeof(info.axisState));
                m_players[playerIndex] = info;
                m_instanceToPlayer[id] = playerIndex;
            }
            break;
        }
        case SDL_CONTROLLERDEVICEREMOVED: {
            SDL_JoystickID id = event.cdevice.which;
            auto it = m_instanceToPlayer.find(id);
            if (it != m_instanceToPlayer.end()) {
                int player = it->second;
                auto playerIt = m_players.find(player);
                if (playerIt != m_players.end()) {
                    SDL_GameControllerClose(playerIt->second.controller);
                    m_players.erase(playerIt);
                }
                m_instanceToPlayer.erase(it);
            }
            break;
        }
        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP: {
            SDL_JoystickID id = event.cbutton.which;
            auto it = m_instanceToPlayer.find(id);
            if (it != m_instanceToPlayer.end()) {
                int player = it->second;
                auto& info = m_players[player];
                if (event.cbutton.button < SDL_CONTROLLER_BUTTON_MAX) {
                    info.buttonState[event.cbutton.button] = (event.type == SDL_CONTROLLERBUTTONDOWN) ? 1 : 0;
                }
            }
            break;
        }
        case SDL_CONTROLLERAXISMOTION: {
            SDL_JoystickID id = event.caxis.which;
            auto it = m_instanceToPlayer.find(id);
            if (it != m_instanceToPlayer.end()) {
                int player = it->second;
                auto& info = m_players[player];
                if (event.caxis.axis < SDL_CONTROLLER_AXIS_MAX) {
                    info.axisState[event.caxis.axis] = event.caxis.value;
                }
            }
            break;
        }
    }
}

bool ControllerManager::IsButtonPressed(int playerIndex, SDL_GameControllerButton button) const {
    std::shared_lock<std::shared_mutex> lock(rw_mutex_);
    auto it = m_players.find(playerIndex);
    if (it != m_players.end() && button < SDL_CONTROLLER_BUTTON_MAX) {
        return it->second.buttonState[button] != 0;
    }
    return false;
}

Uint8 ControllerManager::GetButton(int playerIndex, SDL_GameControllerButton button) const {
    std::shared_lock<std::shared_mutex> lock(rw_mutex_);
    auto it = m_players.find(playerIndex);
    if (it != m_players.end() && button < SDL_CONTROLLER_BUTTON_MAX) {
        return it->second.buttonState[button];
    }
    return 0;
}

Sint16 ControllerManager::GetAxis(int playerIndex, SDL_GameControllerAxis axis) const {
    std::shared_lock<std::shared_mutex> lock(rw_mutex_);
    auto it = m_players.find(playerIndex);
    if (it != m_players.end() && axis < SDL_CONTROLLER_AXIS_MAX) {
        return it->second.axisState[axis];
    }
    return 0;
}

SDL_GameController* ControllerManager::GetControllerForPlayer(int playerIndex) const {
    std::shared_lock<std::shared_mutex> lock(rw_mutex_);
    auto it = m_players.find(playerIndex);
    return (it != m_players.end()) ? it->second.controller : nullptr;
}

int ControllerManager::GetPlayerIndexFromInstanceID(SDL_JoystickID instanceId) const {
    std::shared_lock<std::shared_mutex> lock(rw_mutex_);
    auto it = m_instanceToPlayer.find(instanceId);
    return (it != m_instanceToPlayer.end()) ? it->second : -1;
}

void ControllerManager::RumblePlayer(int playerIndex, Uint16 lowFreq, Uint16 highFreq, Uint32 durationMs) {
    // 直接在调用线程（主线程）执行震动，SDL_GameControllerRumble 是轻量非阻塞的
    SDL_GameController* controller = GetControllerForPlayer(playerIndex);
    if (controller && SDL_GameControllerHasRumble(controller)) {
        SDL_GameControllerRumble(controller, lowFreq, highFreq, durationMs);
    }
}

size_t ControllerManager::GetConnectedPlayerCount() const {
    std::shared_lock<std::shared_mutex> lock(rw_mutex_);
    return m_players.size();
}

int ControllerManager::FindFreePlayerIndex() const {
    // 必须在已持有锁的情况下调用（外部保证）
    int index = 0;
    while (m_players.find(index) != m_players.end()) {
        ++index;
    }
    return index;
}