#ifndef _ELEMENTMANANGER_H_
#define _ELEMENTMANANGER_H_

#include "OpenCore/Runtime/Graphics/IDrawableObject/IDrawableObject.hpp"

// ElementManager.hpp
// 元素管理器类

#include "OpenCore/Runtime/Graphics/UI/BaseBackground.hpp"
#include "OpenCore/Runtime/Graphics/UI/Button.hpp"
#include "OpenCore/Runtime/Graphics/UI/CheckBox.hpp"
#include "OpenCore/Runtime/Graphics/UI/FrameCounter.hpp"
#include "OpenCore/Runtime/Graphics/UI/ImageBoard.hpp"
#include "OpenCore/Runtime/Graphics/UI/MapExplorer.hpp"
#include "OpenCore/Runtime/Graphics/UI/MultiImageBoard.hpp"
#include "OpenCore/Runtime/Graphics/UI/Scrollbar.hpp"

#include "OpenCore/Runtime/Graphics/Sprite/Tile.h"
#include "OpenCore/Runtime/Graphics/Sprite/Waterrect.hpp"

#include "OpenCore/Runtime/Animation/Manager/AnimationManager.hpp"
#include "OpenCore/Runtime/Graphics/IDrawableObject/Sprite.hpp"

#include <memory>
#include <vector>

class ElementManager
{
  public:
    ~ElementManager() { onDestroy(); }

    // 基础方法

    void handlEvents(SDL_Event &event, float totalTime);

    void onUpdate(float totalTime);

    void onRender();

    void onDestroy();

    bool removeElement(const std::string &id);

    bool PushElement(unique_ptr<IDrawableObject> element);

    IDrawableObject *find(const std::string &id)
        const; // 查找是否存在元素的函数，使用const表示不会修改类状态

    unique_ptr<IDrawableObject> getElement(const std::string &id);

    template <typename Func> void forEachElement(Func &&func)
    {
        for (auto &elem : Elements) // 假设内部存放元素的 vector 名为 elements
        {
            if (elem)
                func(elem);
        }
    }

    template <typename Predicate> bool any(Predicate &&pred) const
    {
        for (const auto &elem : Elements)
        {
            if (elem && pred(elem))
                return true;
        }
        return false;
    }

  private:
    // 储存子类的表
    vector<unique_ptr<IDrawableObject>> Elements;
};
#endif //_ELEMENTMANANGER_H_