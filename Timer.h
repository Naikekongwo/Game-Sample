#ifndef _TIMER_H_
#define _TIMER_H_

#include "macro.h"
#include "Timer.h"

class Timer
{
    private:
    int eTime = 0;

    public:
    void onUpdate()
    {
        // 步进算法, 帧数从0到FRAME_RATE
        eTime = (eTime==FRAME_RATE-1)?0:(eTime+1);
    }

    int getTime()
    {
        return this->eTime;
    }

    void setTime(Timer &time)
    {
        eTime = time.getTime();
    }
};

#endif //_TIMER_H_