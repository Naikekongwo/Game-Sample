#ifndef _OPENCORE_PERLINWAVE_HPP_
#define _OPENCORE_PERLINWAVE_HPP_

#include "OpenCore/Concepts/WaveBase.hpp"

// 空的柏林噪声声明，后续可实现
class PerlinWave : public WaveBase
{
public:
    PerlinWave() = default;

    float getHeight(float x, float y, float t) override { return 0.0f; }
    Vec3 getNormalizedVector(float x, float y, float t) override { Vec3 v; v[0]=0; v[1]=0; v[2]=1; return v; }
    float getRange() override { return 0.0f; }
};

#endif //_OPENCORE_PERLINWAVE_HPP_
