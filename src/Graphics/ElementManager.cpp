#include "OpenCore/OpenCore.hpp"
#include <algorithm>

bool ElementManager::find(short id) const
{
    for( auto &element : Elements)
    {
        if(element->getID() == id)
            return true;
            // 找到了
    }
    return false;
    // 未找到     
}

bool ElementManager::PushElement(std::unique_ptr<IDrawableObject> element)
{
    short id = element->getID();
    // 此为当前的ID
    bool existence = find(id);

    if(!existence)
    {
        Elements.push_back(std::move(element));
        // 开始排序
        std::sort(Elements.begin(), Elements.end(), [](const std::unique_ptr<IDrawableObject> &a, const std::unique_ptr<IDrawableObject> &b){ return a->getLayer() < b->getLayer(); });
        SDL_Log("ElementManager::PushElement() successfully pushed a element id %d", id);
        return true;
    }
    
    SDL_Log("ElementManager::PushElement() the element is already existed! %d", id);
    return false;
}

void ElementManager::onUpdate(float totalTime)
{
    for( auto &element : Elements)
    {
        element->onUpdate(totalTime);
    }
}

void ElementManager::handlEvents(SDL_Event &event, float totalTime)
{
    for( auto &element : Elements)
    {
        element->handlEvents(event, totalTime);
    }
}

void ElementManager::onRender()
{
    for( auto &element : Elements)
    {
        element->onRender();
    }
}