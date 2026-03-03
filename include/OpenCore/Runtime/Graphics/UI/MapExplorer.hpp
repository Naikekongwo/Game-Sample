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

#include "OpenCore/Runtime/Graphics/IDrawableObject/UIElement.hpp"

class MapExplorer : public UIElement
{
  public:
    MapExplorer(const string &id, short layer);

    void onEnter() override;
    void onRender() override;
    void onUpdate(float totalTime) override;

    void handlEvents(SDL_Event &event, float totalTime) override;

    // 特殊函数
    void setMap(const string &path) { this->mapPath = path; }

  private:
    uint8_t left_border = 7;
    uint8_t up_border = 4;

    float widthFactor = 1.0f;
    float heightFactor = 1.0f;
    // 临时
    unique_ptr<ImageBoard> renderWorker;
    unique_ptr<ClassicMap> map;
    string mapPath = "maps/paleland.ocmp";
    MapExpStatus status = MapExpStatus::Creating;

    // 临时
    PhysicalProperties pProperties;
};

#endif //_MAP_EXPLORER_HPP_