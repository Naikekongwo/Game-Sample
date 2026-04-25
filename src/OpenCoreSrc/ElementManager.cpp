
#include "OpenCore/OpenCore.hpp"
#include <algorithm>

IDrawableObject *ElementManager::find(const std::string &id) const
{
    for (auto &element : Elements)
    {
        if (element->getID() == id)
            return element.get();
        // 找到了
    }
    return nullptr;
    // 未找到
}

bool ElementManager::PushElement(unique_ptr<IDrawableObject> element)
{
    std::string id = element->getID();
    // 此为当前的ID
    element->onEnter();
    // 进行初始化
    bool existence = find(id);

    if (!existence)
    {
        Elements.push_back(std::move(element));
        // 开始排序
        std::sort(Elements.begin(), Elements.end(),
                  [](const unique_ptr<IDrawableObject> &a,
                     const unique_ptr<IDrawableObject> &b)
                  { return a->getLayer() < b->getLayer(); });
        LOG("成功向元素控制器列表中加入了 {}", id.c_str());
        return true;
    }

    LOG("元素已经存在！元素ID: {}", id.c_str());
    return false;
}

void ElementManager::onUpdate(float totalTime)
{
    for (auto &element : Elements)
    {
        element->onUpdate(totalTime);
    }
}

void ElementManager::handlEvents(SDL_Event &event, float totalTime)
{
    for (auto &element : Elements)
    {
        element->handlEvents(event, totalTime);
    }
}

void ElementManager::onRender()
{
    for (auto &element : Elements)
    {
        element->Draw();
    }
}

unique_ptr<IDrawableObject> ElementManager::getElement(const std::string &id)
{
    if (find(id) == nullptr)
    {
        LOG("获取元素失败，因为其不存在 元素ID: {} "
            "because we cannot find it.",
            id.c_str());
        return nullptr;
    }

    for (auto it = Elements.begin(); it != Elements.end(); ++it)
    {
        if ((*it)->getID() == id)
        {
            auto element = std::move(*it);
            it = Elements.erase(it);
            return element;
        }
    }

    return nullptr;
}

void ElementManager::onDestroy() { Elements.clear(); }

bool ElementManager::removeElement(const std::string &id)
{
    if (!find(id))
    {
        return false;
    }
    for (auto it = Elements.begin(); it != Elements.end(); ++it)
    {
        if (it->get()->getID() == id)
        {
            Elements.erase(it);
            break;
        }
    }
    return true;
}