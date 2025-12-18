#ifndef _OPENCORE_SINEWAVE_HPP_
#define _OPENCORE_SINEWAVE_HPP_

#include "OpenCore/Concepts/WaveBase.hpp"
#include <numbers>

class SineWave : public WaveBase
{
public:
    SineWave(float A, float wL, float fre, float pha, float vX, float vY);

    float getHeight(float x, float y, float t) override;
    Vec3 getNormalizedVector(float x, float y, float t) override;
    float getRange() override;

private:
    void RefreshInfo();

    float amplitude;
    float waveLength;
    float frequency;
    float phase;
    float vectorX;
    float vectorY;

    float omega;
    float normX;
    float normY;
    float waveCount;
};

#endif //_OPENCORE_SINEWAVE_HPP_
