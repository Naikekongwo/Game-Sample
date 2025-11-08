#ifndef _ELEMENTMANANGER_H_
#define _ELEMENTMANANGER_H_

#include "IDrawableObject.hpp"

// ElementManager.hpp
// 元素管理器类

#include "UI/Button.hpp"
#include "UI/ImageBoard.hpp"
#include "UI/BaseBackground.hpp"
#include "UI/FrameCounter.hpp"

#include <vector>
#include <memory>

template <typename T>
std::unique_ptr<T> UI(std::string id, short layer, short texID, short frameX, short frameY);

// 引入工厂方法
#include "UI/UIFactory.inl"

class ElementManager
{
    public:

    // 基础方法

    void handlEvents(SDL_Event &event, float totalTime);

    void onUpdate(float totalTime);

    void onRender();

    bool PushElement(std::unique_ptr<IDrawableObject> element);

    IDrawableObject* find(std::string id) const; // 查找是否存在元素的函数，使用const表示不会修改类状态

    private:
    // 储存子类的表
    std::vector<std::unique_ptr<IDrawableObject>> Elements;
};
#endif //_ELEMENTMANANGER_H_