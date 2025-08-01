#ifndef _TIMER_H_
#define _TIMER_H_

// Timer.h
// 定时器类

#include <chrono>

class Timer
{
    public:

    Timer(int TargetFrameRate);
    
    void Tick();
    
    double getDeltaTime() const { return deltaTime; }
    float getDelayTime() const { return (frameLimit-deltaTime>0)?frameLimit-deltaTime:0;}
    float getTotalTime() const { return totalTime; }
    float getStandardTime() const { return frameLimit; }

    private:
    std::chrono::high_resolution_clock::time_point startTime;// 起始时间点
    std::chrono::high_resolution_clock::time_point lastTime;// 上一帧时间点
    float deltaTime = 0.0f;// 帧间隔时间
    float totalTime = 0.0f;
    float frameLimit = 0.0f;
    int targetFPS = 30;
};

#endif //_TIMER_H_