/**
 * @file DrawableConfigurator.hpp
 * @author Naikekongwo
 * @brief
 * @version 1.0
 * @date 2025-08-07
 *
 * @copyright Copyright (c) 2026
 *
 */

#ifndef _DRAWABLECONFIGURATOR_H_
#define _DRAWABLECONFIGURATOR_H_

#include <cstdint>

enum class AnchorPoint : int;

class IDrawableObject;

class DrawableConfigurator
{
  public:
    // 构造函数
    DrawableConfigurator(IDrawableObject *obj) : obj_p(obj) {}

    DrawableConfigurator &Scale(float w, float h);

    DrawableConfigurator &Anchor(AnchorPoint anchor);
    DrawableConfigurator &Posite(float xPercent, float yPercent);

    DrawableConfigurator &Sequence(bool sequential);

    DrawableConfigurator &Parent(IDrawableObject *parent);

    DrawableConfigurator &Alpha(float alphaValue);

    DrawableConfigurator &Follow(int margin);

  private:
    IDrawableObject *obj_p;
};

#endif //_DRAWABLECONFIGURATOR_H_