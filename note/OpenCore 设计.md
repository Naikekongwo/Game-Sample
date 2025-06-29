# OpenCore Engine 设计大纲

*2025年3月12日 刘绍阳*

----

### 一、文件结构


>/ Game-Sample Project <br>
|---- Game-Sample.cpp // 入口源码 <br>
|---- openCore.cpp / openCore.h // 引擎源代码 <br>
|---- stateController.cpp / stateController.h // 有限状态机管理器 <br>
|---- states // 状态 <br>
|-------- state.h // 抽象的状态基类 <br>
|-------- preload.cpp / preload.h // 启动页面状态<br>
|-------- mainMenu.cpp / mainMenu.h // 主页面状态对象<br>
|-------- gameSettings.cpp / gameSettings.h // 设置页面状态 <br>
|---- assets // 材质资源文件夹 <br>
----


#### `state.h 派生子类` stateController 逻辑详解

在此我默认各位已经了解了`Mainloop()`函数的作用，想必各位也一定好奇，游戏从启动之初的加载页面（展示开发者信息，播放PV的页面），到主页面，以及游戏中暂停打开设置页面，或者是从游戏内退出回到主页面——这些操作无一例外是在`Mainloop()`函数中的循环中进行的，那么究竟应该怎样设计结构，才能用一个易于理解的逻辑实现这一切动作呢？

答案便是**有限状态机(Finite State Machine)**，在此不深入探讨其原始含义，仅仅探讨其在游戏开发中的实际应用。

我们大可将游戏中的不同状态分为以下几种。
+ Preload 加载页面状态
+ Menu 菜单页面状态
+ Game 游戏内状态
+ Pause 暂停页面状态
+ Settings 设置页面状态

我们要设计一个基类，作为这几大状态的抽象类的父类，于是`state.h`便呼之欲出。无论在哪个状态下，我们都要考虑键盘输入，画面的渲染，以及每一刻的事件同步和更新机制，这是**状态**这个抽象概念的共性。我们在抽象类`state.h`中声明虚函数`handlEvent()` `onUpdate()` `onDraw()`三大方法，具体的定义我们将在各个状态的独立的源码文件中给出，在此只阐述他们大体的功能。

```C++
class State
{
    public:
    virtual ~State() {};

    virtual void handlEvents(SDL_Event &event) = 0;
    // 接受传入的事件并处理
    virtual void onUpdate() = 0;
    // 在mainloop每次刷新时，同步更新本帧的内容
    virtual void render(SDL_Renderer* renderer) = 0;
    // 渲染方法，用这个方法渲染本帧内容
};
```

你已经注意到了，我们只设计了状态，还没有设计管理状态的`状态机管理器（stateController）`。其大致设计方向便是，将状态地址作为其私有对象管理，甚至可以使用**vector**同时储存多个状态。

```C++
class StateManager
{
    private:
    vector<State*> states;

    public:
    ~StateManager()
    {
        // 状态管理器的析构函数，遍历销毁所有状态
        for(auto state : states) delete state;
    }

    void pushState(State* state) { states.push_back(state); }

    void popState()
    {
        if (!states.empty()) {
            delete states.back();
            states.pop_back();
        }
    }

    // ········省略········

    
};
```
----
`gameState() 游戏状态` **至关重要的实现完整游戏运转的**