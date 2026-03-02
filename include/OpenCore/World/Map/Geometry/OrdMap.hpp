#pragma once

// OrdMap.hpp
// 标准的二维地图

#include "OpenCoreMap.hpp"

class OrdMap : public OpenCoreMap
{
  public:
    void Init() override;
    void onUpdate(float totalTime) override;
    void onRender() override;

    ~OrdMap() {}

  private:
    uint8_t mapWidth = 0;
    uint8_t mapHeight = 0;
};