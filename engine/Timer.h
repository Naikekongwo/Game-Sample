#ifndef _TIMER_H_
#define _TIMER_H_

// Timer.h 计时器

#include "../macro.h"
#include "Timer.h"

class Timer
{
    private:
    int eTime = 0;
    int maxmium = FRAME_RATE;

    public:
    void onUpdate()
    {
        eTime = (eTime==maxmium-1)?0:(eTime+1); // 步进算法, 帧数从0到FRAME_RATE
    }

    int getTime()
    {
        return this->eTime;
    }

    void setTime(int eTime)
    {
        this->eTime = eTime;
    }

    void setMaxmium(int Max)
    {
        this->maxmium = Max;
    }
};

#endif //_TIMER_H_