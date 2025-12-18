#ifndef _OPENCORE_WAVE_HPP_
#define _OPENCORE_WAVE_HPP_

#include "OpenCore/Concepts/OpenCore_Vec3.hpp"
#include <bit>
#include <numbers>


class WaveInfo
{
    public:
    
    float amplitude;
    float waveLength;
    float frequency;
    float phase;
    float vectorX;
    float vectorY;

    // 分别为
    // 振幅/波长/频率/相位/X方向向量/Y方向向量

    float omega;
    float normX;
    float normY;
    float waveCount;
    
    WaveInfo(float A, float wL, float fre, float pha, float vX, float vY) : amplitude(A), waveLength(wL), frequency(fre), phase(pha), vectorX(vX), vectorY(vY)
    {
        RefreshInfo();
    }

    // 刷新角速度、X单位和Y单位
    void RefreshInfo()
    {
        omega = 2 * std::numbers::pi * frequency;
        
        float length = sqrt(vectorX * vectorX + vectorY * vectorY);

        if(length == 0.0f) length = 1.0f;

        normX = vectorX / length;
        normY = vectorY / length;

        waveCount = 2 * std::numbers::pi / waveLength;
    }

};

class OpenCore_Wave
{
    public:

    OpenCore_Wave() = default;


    void insertWave(WaveInfo waveInfo)
    {
        Wave_.push_back(waveInfo);
    }

    // 获取高度
    float getHeight(float x, float y, float t)
    {
        float summarize = 0.0f;

        for(auto &entry : Wave_)
        {
            float insideFactor = 0.0f;
            
            insideFactor+=entry.phase;
            insideFactor+=entry.normX * entry.waveCount * x;
            insideFactor+=entry.normY * entry.waveCount * y;
            insideFactor-=entry.omega * t;

            summarize+=entry.amplitude * sin(insideFactor);
        }

        return summarize;
    }

    // 获取梯度/法向量
    Vec3 getNormalizedVector(float x, float y, float t)
    {
        Vec3 normalizedVec;
        normalizedVec[2] = 1;

        for(auto &entry : Wave_)
        {
            float insideFactor = 0.0f;

            insideFactor+=entry.phase;
            insideFactor+=entry.normX * entry.waveCount * x;
            insideFactor+=entry.normY * entry.waveCount * y;
            insideFactor-=entry.omega * t;

            normalizedVec[0]-=entry.amplitude * cos(insideFactor) * entry.normX * entry.waveCount;
            normalizedVec[1]-=entry.amplitude * cos(insideFactor) * entry.normY * entry.waveCount;
        }

        normalizedVec.normalize();

        return normalizedVec;
    }

    float getRange()
    {
        float range = 0.0f;
        for(auto &entry : Wave_)
        {
            range+=(entry.amplitude > 0) ? entry.amplitude : -entry.amplitude;
        }

        return range;
    }

    private:

    vector<WaveInfo> Wave_;
};

#endif //_OPENCORE_WAVE_HPP_