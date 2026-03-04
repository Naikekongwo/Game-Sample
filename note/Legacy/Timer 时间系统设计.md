# Timer System Designing Idea <br> 时间系统设计

刘绍阳 二零二五年七月二十六日


### 一、类设计

```C++
class Timer
{
    public:
    void Tick();

    // 计算本帧到上一帧的间隔时间
    double getDeltaTime();
    // 计算本帧剩余的时间
    float getDelayTime();

    private:
    std::chrono::high_resolution_clock::time_point lastTime;

    float deltaTime = 0.0f;
    float frameLimit = 0.033f;    
}
```
以上即为Timer类的结构，可见，有三个公有成员：`Tick()` `getDeltaTime()` `getDelayTime()`，接下来便从这几者各自的功能来谈起

### 二、函数的概念

#### `Tick()` 帧点函数
这个函数每帧会执行一次，既计算本帧离上一帧经过的时间，也会记录当前的时间，提供给下一帧再次调用的时候计算。其原理就是使用C++标准库中的`chrono库`来获取精确的时间参与计算。

#### `getDeltaTime()` 获取Δ时间
在上一个函数中，我们只计算了时间间隔，并将间隔保存到了私有成员`(float)deltaTime`之中去了，还没有返回，于是可以通过这个函数来获取。*Warning: 不能返回负值！*

#### `getDelayTime()` 获取延迟时间
你可能好奇，我们大家都有可能好奇，游戏应该通过怎样的手段才能维持固定的帧数 **60HZ/90HZ/120HZ**呢？其解决方案是很原始的，既然我们计算了帧间隔，我们可以计算出某个固定的刷新率情况之下所对应的帧的固定时长，比如30HZ每一帧的固定帧长为1/30 = 0.033s也就是33ms。这样，我们通过计算帧间隔，如果这一帧目前还没用到33ms，那我们等待剩余的时间，直到33ms，如果已经超过了，我们就跳帧，直接进入下一帧。

当然这都是建立在一个共识上，也就是大多数情况下都有剩余，不然就会出现溢出。