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
        return true;
    }
    return false;
}

bool EventManager::WaitEvent(Event& event) {
    SDL_Event sdlEvent;
    if (SDL_WaitEvent(&sdlEvent)) {
        event = Event(sdlEvent);
        return true;
    }
    return false;
}

void EventManager::PushEvent(const Event& event) {
    SDL_Event sdlEvent = event.GetSDLEvent();
    SDL_PushEvent(&sdlEvent);
}

