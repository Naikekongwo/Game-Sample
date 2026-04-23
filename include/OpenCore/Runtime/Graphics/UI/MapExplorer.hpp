/**
 * @file MapExplorer.hpp
 * @author your name (you@domain.com)
 * @brief 地图浏览器类的头文件
 * @version 0.1
 * @date 2026-04-23
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef _MAP_EXPLORER_HPP_
#define _MAP_EXPLORER_HPP_

enum class MapExpStatus
{
    Creating,
    Ready
};

class ClassicMap;

class ImageBoard;

#include "OpenCore/Runtime/Gameplay/Physics/PhysicalProperties.h"
#include "OpenCore/Runtime/Gameplay/WorldController/IWorldController.hpp"
#include "OpenCore/Runtime/Graphics/IDrawableObject/UIElement.hpp"

/**
 * @brief 地图浏览器类
 *
 */
class MapExplorer : public UIElement
{
  public:
    MapExplorer(const string &id, short layer);

    void onEnter() override;

    // 渲染函数（新）
    void Draw() override;
    void onUpdate(float totalTime) override;

    void onExit() override;

    void handlEvents(SDL_Event &event, float totalTime) override;

  private:
    float widthFactor = 1.0f;
    float heightFactor = 1.0f;

    MapExpStatus status = MapExpStatus::Creating;
};

#endif //_MAP_EXPLORER_HPP_