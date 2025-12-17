#ifndef _ELEMENTMANANGER_H_
#define _ELEMENTMANANGER_H_

#include "OpenCore/Graphics/IDrawableObject/IDrawableObject.hpp"

// ElementManager.hpp
// 元素管理器类

#include "OpenCore/Graphics/UI/BaseBackground.hpp"
#include "OpenCore/Graphics/UI/Button.hpp"
#include "OpenCore/Graphics/UI/FrameCounter.hpp"
#include "OpenCore/Graphics/UI/ImageBoard.hpp"
#include "OpenCore/Graphics/UI/Label.hpp"


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

    bool PushElement(std::unique_ptr<IDrawableObject> element);

    IDrawableObject *find(const std::string &id)
        const; // 查找是否存在元素的函数，使用const表示不会修改类状态

    std::unique_ptr<IDrawableObject> getElement(const std::string &id);

  private:
    // 储存子类的表
    std::vector<std::unique_ptr<IDrawableObject>> Elements;
};
#endif //_ELEMENTMANANGER_H_