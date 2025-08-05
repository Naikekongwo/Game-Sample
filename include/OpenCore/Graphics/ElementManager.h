// ElementManager.h
// 元素管理器类
#ifndef _ELEMENTMANANGER_H_
#define _ELEMENTMANANGER_H_

#include "IDrawableObject.h"

#include "UI/Button.h"
#include "UI/ImageBoard.h"

#include <vector>
#include <memory>

class ElementManager
{
    public:

    // 基础方法

    void handlEvents(SDL_Event &event, float totalTime);
    void onUpdate(float totalTime);

    void onRender();

    bool PushElement(std::unique_ptr<IDrawableObject> element);

    bool find(short id);

    private:
    // 储存子类的表
    std::vector<std::unique_ptr<IDrawableObject>> Elements;
};
#endif //_ELEMENTMANANGER_H_