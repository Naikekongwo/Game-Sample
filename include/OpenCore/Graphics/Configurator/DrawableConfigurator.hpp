#ifndef _DRAWABLECONFIGURATOR_H_
#define _DRAWABLECONFIGURATOR_H_

// DrawableConfigurator.h
// 初始化所有可绘制对象（控件）的部署器类

#include <cstdint>

enum class AnchorPoint : int;

class IDrawableObject;

class DrawableConfigurator
{
    public:
    // 构造函数
    DrawableConfigurator(IDrawableObject* obj) : obj_p(obj) {}

    DrawableConfigurator& Scale(uint16_t w, uint16_t h);

    DrawableConfigurator& Anchor(AnchorPoint anchor);
    DrawableConfigurator& Posite(int16_t x, int16_t y );

    DrawableConfigurator& Sequence(bool sequential);

    DrawableConfigurator& Parent(IDrawableObject* parent);

    private:
    IDrawableObject* obj_p;
};


#endif //_DRAWABLECONFIGURATOR_H_