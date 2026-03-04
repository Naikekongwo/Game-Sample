# OpenCore Tutorial <br> 开发指南 <br> for OpenCore 26.5


## 目录

- [OpenCore Tutorial  开发指南  for OpenCore 26.5](#opencore-tutorial--开发指南--for-opencore-265)
  - [目录](#目录)
    - [Hello World 创建你的第一个游戏](#hello-world-创建你的第一个游戏)
      - [`struct GameInfo` 注册你的游戏](#struct-gameinfo-注册你的游戏)
      - [`OpenEngine` 点火·启动·引擎的使用](#openengine-点火启动引擎的使用)
    - [EntityRegister 实体注册机制](#entityregister-实体注册机制)



### Hello World 创建你的第一个游戏

#### `struct GameInfo` 注册你的游戏

为了为多种游戏提供力所能及的引擎级支持，OpenCore大量采用了注册机制来初始化一些信息和功能，如果你要创建你个人的第一款游戏，那么你必须考虑到注册游戏的相关信息。

当然，在万事之前，我们先要进行头文件的引用。

``` C++
#include "OpenCore/OpenCore.h"
```

从这里开始，我们便引用了引擎的头文件，获得了引擎的基本数据结构支持。接下来我们正式的来创建GameInfo，首先我们一览GameInfo的结构体定义：

```C++
struct GameInfo
{
    string gameName = "openengine-game";
    uint8_t version_major = 1;
    uint8_t version_minor = 0;
    bool beta = false;
    unique_ptr<Stage> entranceStage;

    int TargetResolutionWidth = 1920;
    int TargetResolutionHeight = 1080;

    bool nearestScaling = true;
};
```

| 类型                | 成员名          | 功能                     |
| ------------------- | --------------- | ------------------------ |
| `string`            | gameName        | 游戏的名称               |
| `uint8_t`           | version_major   | 游戏的大版本号           |
| `uint8_t`           | version_minor   | 游戏的小版本号           |
| `bool`              | beta            | 是否为测试版             |
| `unique_ptr<Stage>` | entranceStage   | 入口场景                 |
| `int`               | Target...Width  | 目标分辨率宽度           |
| `int`               | Target...Height | 目标分辨率高度           |
| `bool`              | nearestScaling  | 缩放方式（是否启用邻近） |

这里面有许多顾名思义的成员，游戏名称不必多说，版本号也是非常常见和普遍的内容，beta用于判断是否为测试版（如果是测试版，将会显示不同的信息，同时允许打印LOG）。两个用于存储分辨率的int型数据也是顾名思义的，如果你希望开发目标4K平台的游戏，那么你需要将这两者分别设置为 **3840** x **2160**，反之，如果你想要开发1080P平台的游戏，你便将其设定为 **1920** x **1080**。在`OpenCore Eclipsea 25.7`版本之后，引擎已经引入了较为完备的相对尺寸系统，因此往日使用绝对坐标导致的分辨率差异已经不复存在。

我们以OpenCore演示游戏`Eclipsea 无水之境`的GameInfo创建流程为例：

```C++
 auto eclipseaInfo = std::make_unique<GameInfo>(
        "Eclipsea", 1, 2, true,
        std::make_unique<PreloadStage>(timer, sController), 1920, 1080, true);
```

这段代码所创建的GameInfo的含义如下：
- 游戏名称： Eclipsea
- 游戏版本: 1.2
- 游戏为测试版：是
- 游戏入口场景: PreloadStage
- 游戏分辨率: 1920 x 1080
- 游戏采取邻近放大：是

当然，教程到此时还未介绍Stage或是PreloadStage，我们可以先将其认为是一种场景，对于Hello World项目来说，我们只需要这样写:

```C++
std::make_unique<DemoStage>(timer, sController);
```

DemoStage是引擎内置的一个示例页面，作为创建第一个游戏的我们，不需要从一开始就制作自己的Stage，因此，我们可以直接使用这个类作为我们GameInfo中entranceStage的参数。

完整的代码如下：

```C++
auto gameInfo = std::make_unique<GameInfo>("HelloWorld",1,0,true, std::make_unique<DemoStage>(timer, sController), 1920, 1080, false);
```

#### `OpenEngine` 点火·启动·引擎的使用

我们在上一小节已经声明了一个gameInfo的特殊指针（其类型为unique_ptr<GameInfo>），你可能会发现，现在timer、sController的下方都浮动着波浪线（大部分时刻是红色的，代表未定义）。

这代表你的编辑器其实没有找到这两者的定义，这其实是合理的。

这两者分别代表：`计时器` `舞台管理器`，这两者是由OpenCore Engine类管理其生命周期的，你在此处并未获取他们的指针，所以未定义是合理的。

想要获取其地址，只需：

```C++
(void)OpenEngine::getInstance();
OpenEngine &engine = OpenEngine::getInstance();

if (!engine.Initialize())
{
    // 初始化引擎
    Console_Log("Engine initialization was failed.");
    return false;
}

auto timer = engine.getTimer();
auto sController = engine.getStageController();
```

**注意：将他们放到我们声明gameInfo之前**

这些语句的作用是，初始化了OpenEngine的单例对象，然后获取了这个单例对象的引用，并调用了OpenEngine的初始化方法，接着获取了计时器和舞台管理器。

至此，gameInfo的构建和引擎的初始化已经顺利完成。但这充其量只能谈得上刚刚发动了汽车，想要开起来，还要进行接下来的几步：

```C++
try
{
    engine.MainLoop();
    engine.CleanUp();
}
catch (std::exception e)
{
    Console_Log("The engine exited with error : %s", e.what());
    return false;
}

return true;
```

一个十分典型的 try-catch 语句，在这个语句之中，我们调用了engine的主循环方法，也就相当于松开了刹车，踩下了油门（当然自动挡），引擎正式开始运行了——如果运行中出现任何错误，会立即捕捉exception并打印。

如果没有问题的话，点生成-调试，你可以在屏幕上看到Hello World。

### EntityRegister 实体注册机制