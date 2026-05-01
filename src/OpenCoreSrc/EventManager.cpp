#include "OpenCore/Core/Event/EventManager.hpp"
#include <SDL2/SDL.h>



// 单例实现
EventManager& EventManager::GetInstance() {
    static EventManager instance;
    return instance;
}

bool EventManager::Init() {
    // SDL_StartTextInput();
    return true;
}

void EventManager::Shutdown() {
    // SDL_StopTextInput();
}

bool EventManager::PollEvent(Event& event) {
    SDL_Event sdlEvent;
    if (SDL_PollEvent(&sdlEvent)) {
        event = Event(sdlEvent);
        UpdateInputMode(event);  
        return true;
    }
    return false;
}

bool EventManager::WaitEvent(Event& event) {
    SDL_Event sdlEvent;
    if (SDL_WaitEvent(&sdlEvent)) {
        event = Event(sdlEvent);
        UpdateInputMode(event);  
        return true;
    }
    return false;
}

void EventManager::PushEvent(const Event& event) {
    SDL_Event sdlEvent = event.GetSDLEvent();
    SDL_PushEvent(&sdlEvent);
}

InputMode EventManager::GetInputMode() const {
    return m_inputMode;
}

void EventManager::SetInputMode(InputMode mode) {
    m_inputMode = mode;
}

bool EventManager::GetAllowtoDivertMod() const {
    return allowtoDivertMod==1;
}

void EventManager::SetAllowtoDivertMod(bool allowtoDivertMod) {
    if(allowtoDivertMod)
    this->allowtoDivertMod = 1;
    else if(!allowtoDivertMod)
    this->allowtoDivertMod = 0;
}

void EventManager::UpdateInputMode(const Event& event) {
    if (!GetAllowtoDivertMod()) {return;}
    Uint32 type = event.GetType();
    if (type == SDL_CONTROLLERAXISMOTION     ||
        type == SDL_CONTROLLERBUTTONDOWN     ||
        type == SDL_CONTROLLERBUTTONUP       ||
        type == SDL_CONTROLLERDEVICEADDED    ||
        type == SDL_CONTROLLERDEVICEREMOVED  ||
        type == SDL_CONTROLLERDEVICEREMAPPED) {
        m_hasGamepadEvent = true;
    } else if (type == SDL_KEYDOWN      || type == SDL_KEYUP ||
               type == SDL_MOUSEMOTION  ||
               type == SDL_MOUSEBUTTONDOWN || type == SDL_MOUSEBUTTONUP ||
               type == SDL_MOUSEWHEEL) {
        m_hasKMEvent = true;
    }
    
}

void EventManager::EndFrame() {
    if (!GetAllowtoDivertMod()) {return;}
    if (m_hasGamepadEvent && !m_hasKMEvent)
        m_inputMode = InputMode::Gamepad;
    else if (m_hasKMEvent && !m_hasGamepadEvent)
        m_inputMode = InputMode::KeyboardMouse;
    // 两者都有或都没有 → 不改变

    m_hasKMEvent = false;
    m_hasGamepadEvent = false;
}
