#ifndef _OPENCORE_WAVEBASE_HPP_
#define _OPENCORE_WAVEBASE_HPP_

#include "OpenCore/Concepts/OpenCore_Vec3.hpp"

class WaveBase
{
public:
    virtual ~WaveBase() = default;

    // 返回该波形在 (x,y,t) 的高度（单个波的贡献）
    virtual float getHeight(float x, float y, float t) = 0;

    // 返回该波形对法线/梯度的贡献（x,y 分量为切向量贡献，z 分量可为 0）
    virtual Vec3 getNormalizedVector(float x, float y, float t) = 0;

    // 返回该波形的振幅范围（用于累计总范围）
    virtual float getRange() = 0;
};

#endif //_OPENCORE_WAVEBASE_HPP_
