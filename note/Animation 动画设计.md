# Animation System Design <br> 动画系统设计

> 刘绍阳 二零二五年八月一日

### 一、结构设计
#### 1.1 类的设计
动画的数据结构是复杂而多变的，但其功能却保持高度一致，因而绝不能使用离散的类来定义。故我们先定义一个基类。
```C++
class IAnimation
{
    public:
    virtual ~IAnimation() = default;

    virtual void onUpdate(float totalTime, AnimationState& state) = 0;

    virtual bool isFinished() const = 0;
    virtual void reset(float totalTime, AnimationState& state) = 0;

    virtual bool isLoop() = 0;
};
```

同样的，就像我们在Stage系统设计中一般，我们的基类保持一个纯虚类的存在。

其中，`onUpdate()`函数的功能和我们在游戏开发的其他位置所见的是一致的。而`isFinished()`则负责返回动画的执行状态，动画动画，动画一定是动态而且拥有时间尺度的。`isLoop()`函数负责返回该动画是否是循环动画。`reset()`则负责将动画归位。

根据我们基类的设计，我们可以将子类分为两种动画类，一种是**FrameAnimation** 另一种则是 **PropertyAnimation**。前者是多帧动画的播放，而后者是针对单帧图片而言的，是对其属性的动态演化：例如缩放动画(ScaleAnimation)，旋转动画(RotateAnimation)还有位移动画(MoveAnimation)。

对于其他类动画，在此我不赘述，暂时只深入讲一下我所设计的模板动画，也就是FrameAnimation的类设计：

```C++
class FrameAnimation : public IAnimation
{
    public:
    FrameAnimation(int totalFrames, int FPS, bool isLooping);
    // 构造方法

    void onUpdate(float totalTime, AnimationState& state) override;
    // 更新动画状态

    bool isFinished() const override;
    // 检查动画是否完成

    void reset(float totalTime, AnimationState& state) override;
    // 重置动画状态

    bool isLoop() override { return isLooping; }
    // 检查动画是否循环

    private:
    int currentFrame; // 当前帧索引
    int totalFrames; // 总帧数
    int FPS; // 帧率
    
    bool isLooping; // 是否循环
};
```

多么庞大的类！

我们拥有一个构造函数，其中传入了一个总帧数(totalFrames)参数和每秒帧数（FPS）以及是否是循环动画的布尔值（isLooping）。

同样的，他完整继承了父类的的各项函数，例如`onUpdate()` `isFinished()` `reset()` `isLoop()`。那么，我们的动画类是怎么跟随时间动态跟进而修改动画进度的呢？

首先是输入的时间，我们可以看到onUpdate函数输入了一个时间和**AnimationState**的引用对象。在此就不赘述AnimationState结构体的具体成员，只提到一点：这个对象包含了动画开始的时间，以及动画对应的帧数（一组图片里对应的序号），图片的缩放比例，透明度等等等等。

这样来说，我们的动画类正是通过输入的时间和动画开始的时间计算出时间差来算出现在动画正进行到的位置，并且修改AnimationState中的对应参数来进行修改的。

为什么要让AnimationState对象由外界对象持有，而只作为一个参数传入动画类呢？这一点是为了一个对象能够同时持有多个动画效果而做的，让AnimationState独立出来，这样就可以在多个动画的onUpdate函数中来回传递。我们可以实现，例如：闪烁+变大等等的效果。