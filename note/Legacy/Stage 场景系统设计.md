# Stage Controller Designing Idea <br> 场景系统设计

刘绍阳 二零二五年七月二十六日


### 一、类设计

```C++
class Stage{
    public:
    ~Stage() = default;

    // 虚函数部分

    // 初始化逻辑
    virtual bool handlEvents(SDL_Event* event) = 0;

    // 更新逻辑
    virtual void onUpdate() = 0;

    // 渲染逻辑
    virtual void onRender() = 0;


    protected:
    SDL_Renderer* renderer = nullptr; //渲染器
    ResourceManager* resourceManager = nullptr; // 资源管理器
    SoundEffectManager* sfxManager = nullptr; // 音效管理器
};
```
以上即为Stage类的结构，可见在类定义之中，析构函数被声明为默认。而其他的业务逻辑函数，如`handlEvents()` `onUpdate()` `onRender()`都是虚函数。Stage类是纯虚类，这是方便我们构造我们继承自Stage类的具体场景而设计的。

一个游戏可能有多个**Stage**，但是绝对只有一个窗口用于显示，所以，我们不得不去考虑多个场景之间的关系逻辑。如何来协调不同场景和切换？如何管理这些场景？因而我们不得不引入一个新的类：

```C++
class StageController {
    public:
    ~StageController() = default;

    // 切换场景
    void changeStage(std::unique_ptr<Stage> newStage);

    // 处理事件的公共接口
    bool handlEvents(SDL_Event* event);

    // 更新逻辑
    void onUpdate();
    
    // 渲染逻辑
    void onRender();
    
    private:
    std::unique_ptr<Stage> currentStage; // 当前场景
};
```

是的，这便是**StageController类**。他有一个智能指针（ `unique_ptr<>` ）私有成员**currentStage**，这个对象储存着当前场景的地址。为何要使用智能指针，而不是裸指针呢？因为从逻辑上来进行判断，场景一定是被场景管理器全权管辖的，因而场景控制器对其拥有所有权，而且在业务逻辑中我们一定会遇到场景的切换，如何来协调上一个场景和下一个场景，在此处使用智能指针就非常好，因为切换场景时，智能指针会自动回收上一个场景地址的资源，这为我们免除了手动资源回收的繁琐步骤。

### 二、函数的概念

#### `Stage::handlEvents(SDL_Event* event) 函数`
望文生义，所谓handlEvents函数，就是用来handle events的函数。这个函数传入一个SDL_Event类型的指针，然后在函数内部，根据不同场景的具体需要，来对event进行所谓的逻辑判断。

举个例子，例如说，不同页面（场景）之中对同一事件的定义不同，实际来讲，例如主页面按ESC可能会触发退出逻辑，但是在游戏内按ESC可能会触发暂停页面，这两者对同一事件的处理不同，这就是为什么我们需要在Stage的内部来定义各自对事件的处理逻辑的原因。

#### `Stage::onUpdate(SDL_Event* event) 函数`
更新函数的概念广泛存在于各类游戏编程之中，属于游戏编程的经典概念。这个函数的触发是有事件规律的，例如说，每秒钟触发30次。

而对于每次触发来说，相对而言在单次触发内部，Stage或者其他含有onUpdate函数的对象，都保持着一种静态。是的，我们在游戏开发中所设计的这些对象并不是活的，他需要有逻辑让他从静态变成动态——即活的对象。我们可以在函数里来对当前对象的运动状态做判断，进而从当下这个静止的状态来计算出下一状态的各项属性，这就是如何让对象动态变化的关键。

对Stage类具体而言，我们可以这样说，他的onUpdate函数是用于判断场景状态的。而具体这个状态要怎么定义，则是看我们要如何使用这个特性了。它可以是确定同一场景的不同状态，例如说PreloadStage的未加载和加载完成状态。也可以是GameplayStage的运动和非运动状态。

#### `Stage::onRender(SDL_Event* event) 函数`
这个函数是用于渲染场景的，他的调用也有规律，但是不一定和onUpdate的时间频率相同。我们对画面的渲染可能需要120FPS，但是onUpdate可能只需要30FPS，这是一种节能的想法。

---
#### `StageController::ChangeStage()`
切换场景的函数，因为智能指针的引入，在判断当前场景并非传入的场景地址之后，就会使用unique_ptr的move方法，自动销毁被替代的场景。

#### `StageController::onRender() & onUpdate() & handlEvents()`
这三个函数对于场景管理器的意义就是将引擎传来的要求下发给**currentStage**，只是作为中继函数。