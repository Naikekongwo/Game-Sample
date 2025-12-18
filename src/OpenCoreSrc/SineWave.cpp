#include "OpenCore/Concepts/SineWave.hpp"
#include <cmath>

SineWave::SineWave(float A, float wL, float fre, float pha, float vX, float vY)
    : amplitude(A), waveLength(wL), frequency(fre), phase(pha), vectorX(vX), vectorY(vY)
{
    RefreshInfo();
}

void SineWave::RefreshInfo()
{
    omega = 2 * std::numbers::pi * frequency;

    float length = sqrt(vectorX * vectorX + vectorY * vectorY);

    if(length == 0.0f) length = 1.0f;

    normX = vectorX / length;
    normY = vectorY / length;

    waveCount = 2 * std::numbers::pi / waveLength;
}

float SineWave::getHeight(float x, float y, float t)
{
    float insideFactor = 0.0f;
    insideFactor += phase;
    insideFactor += normX * waveCount * x;
    insideFactor += normY * waveCount * y;
    insideFactor -= omega * t;

    return amplitude * sin(insideFactor);
}

Vec3 SineWave::getNormalizedVector(float x, float y, float t)
{
    Vec3 contrib;
    float insideFactor = 0.0f;
    insideFactor += phase;
    insideFactor += normX * waveCount * x;
    insideFactor += normY * waveCount * y;
    insideFactor -= omega * t;

    float c = -amplitude * cos(insideFactor) * waveCount;

    contrib[0] = c * normX;
    contrib[1] = c * normY;
    contrib[2] = 0.0f;

    return contrib;
}

float SineWave::getRange()
{
    return (amplitude >= 0.0f) ? amplitude : -amplitude;
}
