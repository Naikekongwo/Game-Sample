#ifndef _MAP_EXPLORER_HPP_
#define _MAP_EXPLORER_HPP_

// MapExplorer.hpp
// 地图窗口的UI控件

#include "OpenCore/Runtime/Graphics/IDrawableObject/UIElement.hpp"

class MapExplorer : public UIElement
{
  public:
  private:
    // 地图窗口容纳的地图大小
    uint8_t WideRange = 16;
    uint8_t HeightRange = 9;
};

#endif //_MAP_EXPLORER_HPP_