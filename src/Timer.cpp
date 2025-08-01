// 时间类的具体实现

#include "OpenCore/Timer.h"

Timer::Timer(int TargetFrameRate)
{
    targetFPS = TargetFrameRate;
    frameLimit = 1.0f / targetFPS; // 计算每帧的时间限制
}

void Timer::Tick()
{
    // 开始定时器
    using namespace std::chrono;

    // 获取当前的超精确时间
    auto currentTime = high_resolution_clock::now();
    
    if(lastTime.time_since_epoch().count() == 0) {
        // 如果是第一次调用，初始化 lastTime
        lastTime = currentTime;
        startTime = currentTime;
        deltaTime = 0.0f;
        totalTime = 0.0f;
    } else {
        // 计算上一帧到当前帧的时间差
        duration<float> frameDuration = currentTime - lastTime;
        deltaTime = frameDuration.count();
        
        // 更新 lastTime 为当前时间
        lastTime = currentTime;
    }

    duration<float> totalDuration = currentTime - startTime;
    totalTime = totalDuration.count();
}

