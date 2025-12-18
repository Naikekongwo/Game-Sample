#ifndef _OPENCORE_WAVE_HPP_
#define _OPENCORE_WAVE_HPP_


#include "OpenCore/Concepts/OpenCore_Vec3.hpp"
#include "OpenCore/Concepts/WaveBase.hpp"
#include "OpenCore/Concepts/SineWave.hpp"
#include <bit>
#include <numbers>
#include <vector>
#include <memory>


// 为兼容历史代码保留的数据结构，供构造波形使用
class WaveInfo
{
    public:

    float amplitude;
    float waveLength;
    float frequency;
    float phase;
    float vectorX;
    float vectorY;

    // 分别为 振幅/波长/频率/相位/X方向向量/Y方向向量

    WaveInfo(float A, float wL, float fre, float pha, float vX, float vY) : amplitude(A), waveLength(wL), frequency(fre), phase(pha), vectorX(vX), vectorY(vY)
    {
    }
};

class OpenCore_Wave
{
    public:

    OpenCore_Wave() = default;

    // 保持接口兼容，接受历史的 WaveInfo，然后创建对应的 SineWave
    void insertWave(WaveInfo waveInfo)
    {
        Wave_.push_back(std::make_unique<SineWave>(waveInfo.amplitude, waveInfo.waveLength, waveInfo.frequency, waveInfo.phase, waveInfo.vectorX, waveInfo.vectorY));
    }

    // 获取高度（多个子波贡献之和）
    float getHeight(float x, float y, float t)
    {
        float summarize = 0.0f;

        for(auto &entry : Wave_)
        {
            summarize += entry->getHeight(x, y, t);
        }

        return summarize;
    }

    // 获取梯度/法向量（先累计子波的切向贡献，再归一化）
    Vec3 getNormalizedVector(float x, float y, float t)
    {
        Vec3 normalizedVec;
        normalizedVec[0] = 0.0f;
        normalizedVec[1] = 0.0f;
        normalizedVec[2] = 1.0f;

        for(auto &entry : Wave_)
        {
            Vec3 contrib = entry->getNormalizedVector(x, y, t);
            normalizedVec[0] += contrib[0];
            normalizedVec[1] += contrib[1];
        }

        normalizedVec.normalize();

        return normalizedVec;
    }

    float getRange()
    {
        float range = 0.0f;
        for(auto &entry : Wave_)
        {
            range += entry->getRange();
        }

        return range;
    }

    private:

    std::vector<std::unique_ptr<WaveBase>> Wave_;
};

#endif //_OPENCORE_WAVE_HPP_