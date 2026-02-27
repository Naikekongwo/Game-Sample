#pragma once

// Spiral
// 螺旋地图结构

#include <algorithm>
#include <cmath>

#include "OpenCore_Vec2.hpp"

inline int SpiralStepFromCoordinate(const OpenCore_Vec2 &p)
{
    int x = static_cast<int>(p.x);
    int y = static_cast<int>(p.y);

    if (x == 0 && y == 0)
        return 0;

    int layer = std::max(std::abs(x), std::abs(y));
    int sideLength = layer * 2;
    int layerStart = (sideLength - 1) * (sideLength - 1);

    if (y == layer)
        return layerStart + (layer - 1 - x);

    if (x == -layer)
        return layerStart + sideLength + (layer - 1 - y);

    if (y == -layer)
        return layerStart + sideLength * 2 + (x + layer - 1);

    return layerStart + sideLength * 3 + (y + layer - 1);
}

inline OpenCore_Vec2 CoordinateFromSpiralStep(int step)
{
    if (step == 0)
        return OpenCore_Vec2(0.0f, 0.0f);

    int layer = (static_cast<int>(std::sqrt(step - 1)) + 1) / 2;
    int sideLength = layer * 2;
    int layerStart = (sideLength - 1) * (sideLength - 1);

    int offset = step - layerStart;
    int edge = offset / sideLength;
    int pos = offset % sideLength;

    switch (edge)
    {
    case 0:
        return OpenCore_Vec2(layer - 1 - pos, layer);
    case 1:
        return OpenCore_Vec2(-layer, layer - 1 - pos);
    case 2:
        return OpenCore_Vec2(-layer + 1 + pos, -layer);
    default:
        return OpenCore_Vec2(layer, -layer + 1 + pos);
    }
}