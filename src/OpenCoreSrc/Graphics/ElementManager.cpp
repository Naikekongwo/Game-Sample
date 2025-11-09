#include "OpenCore/OpenCore.hpp"
#include "Eclipsea/Eclipsea.hpp"
#include <algorithm>

IDrawableObject* ElementManager::find(const std::string& id) const
{
    for( auto &element : Elements)
    {
        if(element->getID() == id)
            return element.get();
            // 找到了
    }
    return nullptr;
    // 未找到     
}

bool ElementManager::PushElement(std::unique_ptr<IDrawableObject> element)
{
    std::string id = element->getID();
    // 此为当前的ID
    bool existence = find(id);

    if(!existence)
    {
        Elements.push_back(std::move(element));
        // 开始排序
        std::sort(Elements.begin(), Elements.end(), [](const std::unique_ptr<IDrawableObject> &a, const std::unique_ptr<IDrawableObject> &b){ return a->getLayer() < b->getLayer(); });
        SDL_Log("ElementManager::PushElement() successfully pushed a element id %s", id.c_str());
        return true;
    }
    
    SDL_Log("ElementManager::PushElement() the element is already existed! %s", id.c_str());
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

std::unique_ptr<IDrawableObject> ElementManager::getElement(const std::string& id)
{
    if(find(id) == nullptr)
    {
        SDL_Log("ElementManager::getElement() failed to get element : %s, because we cannot find it.", id.c_str());
        return nullptr;
    }
    
    for(auto it = Elements.begin(); it != Elements.end(); ++it)
    {
        if((*it)->getID() == id)
        {
            auto element = std::move(*it);
            it = Elements.erase(it);
            return element;
        }
    }

    return nullptr;
}

void ElementManager::onDestroy()
{
    Elements.clear();
}