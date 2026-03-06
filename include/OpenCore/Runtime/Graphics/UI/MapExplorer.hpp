#ifndef _MAP_EXPLORER_HPP_
#define _MAP_EXPLORER_HPP_

// MapExplorer.hpp
// 地图窗口的UI控件

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

class MapExplorer : public UIElement
{
  public:
    MapExplorer(const string &id, short layer);

    void onEnter() override;
    void onRender() override;
    // 渲染函数（新）
    void Draw() override;
    void onUpdate(float totalTime) override;

    void handlEvents(SDL_Event &event, float totalTime) override;

  private:
    float widthFactor = 1.0f;
    float heightFactor = 1.0f;
    // 临时
    IWorldController worldController;
    MapExpStatus status = MapExpStatus::Creating;
};

#endif //_MAP_EXPLORER_HPP_