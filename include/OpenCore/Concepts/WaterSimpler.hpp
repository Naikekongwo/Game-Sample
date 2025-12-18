#ifndef _WATER_SIMPLER_HPP_
#define _WATER_SIMPLER_HPP_

// WaterSimpler
// 水波纹采样器

#include <algorithm>
#include <cmath>
#include <numbers>
#include <vector>


#include "OpenCore/Concepts/Vec3.hpp"

using std::vector;

// WaveInfo 的格式
// 0 amplitude 振幅
// 1 wavelength 波长
// 2 frequency 频率
// 3 directionX 传播方向的X分量
// 4 directionY 传播方向的Y分量
// 5 phase 初始相位

struct WaveInfo
{
    float amplitude;
    float wavelength;
    float frequency;
    float directionX;
    float directionY;
    float phase;
};

class WaterSimpler
{
  public:
    WaterSimpler() = default;

    void insertWave(WaveInfo waveInfo) { WavePool.push_back(waveInfo); }

    // 增加波形

    float getHeight(float x, float y, float t)
    {
        float val;
        for (auto &entry : WavePool)
        {
            val +=
                entry.amplitude *
                sin(entry.phase + entry.directionX * x + entry.directionY * y +
                    (2 * std::numbers::pi * entry.frequency) * t);
        }

        return val;
    }

    // 获取当前波函数值

    Vec3 getNormalVec(float x, float y, float t)
    {
        Vec3 NormalizedVec;

        NormalizedVec[2] = 1;

        for (auto &entry : WavePool)
        {
            // 进行叠加
            NormalizedVec[0] -=
                entry.amplitude *
                cos(entry.directionX * x + entry.directionY * y +
                    (2 * std::numbers::pi * entry.frequency) * t +
                    entry.phase) *
                entry.directionX;
            NormalizedVec[1]  -=
                entry.amplitude *
                cos(entry.directionX * x + entry.directionY * y +
                    (2 * std::numbers::pi * entry.frequency) * t +
                    entry.phase) *
                entry.directionY;
        }

        NormalizedVec.normalize();

        return NormalizedVec;
    }

    // 求梯度

  private:
    vector<WaveInfo> WavePool;

    // 波形池
};

#endif //_WATER_SIMPLER_HPP_