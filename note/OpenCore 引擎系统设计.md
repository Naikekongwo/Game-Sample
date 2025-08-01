# OpenCore Design <br> 引擎设计

刘绍阳 二零二五年八月一日


### 一、结构设计

#### 1. 类设计

```C++
class OpenEngine
{
    public:
    bool Run();
    bool Initialize();
    bool MainLoop();

    private:
    std::unique_ptr<GFXinstance> gfxInstance;
    std::unique_ptr<SoundEffectManager> sfxManager;
    std::unique_ptr<StageController> sController;
    std::unique_ptr<Timer> timer;

    ResourceManager* resManager; // 这个不用智能指针
};
```
这便是我们引擎类的类声明。

可见，我们拥有三个公共的函数，`Run()` `Initialize()` `MainLoop()`。望文生义地说，这三个函数分别对应了：<u>引擎的主生命流程</u>、<u>引擎初始化函数</u>以及<u>引擎的主循环函数</u>。

他们三者的关系如下：
```C++
bool OpenEngine::Run()
{
    ···
    if(!Initialize()) return false;

    if(!MainLoop()) return false;
    ···
    return true;
}
```

其实，按照经典的游戏流程设计，我们应该还有一个`CleanUp()`函数用于回收、销毁我们在引擎里生产的资源，那为什么这里没有呢？因为我们使用了智能指针 **unique_ptr<>**。这是现代C++的一个新特性，它拥有自我回收的功能，可以见到，我们的引擎类有几个私有成员，分别是`GFXInstance` `SoundEffectManager` `StageController` `Timer`还有`ResourceManager`。前四者都是使用 **unique_ptr** 进行储存的智能指针，最后一者使用的是裸指针来进行储存。

因为智能指针的特性，所以我们不需要再对这四者进行手动回收了。而ResourceManager虽然使用的是裸指针进行保存，但由于其是单例（后面会提到），因而它的销毁也是自动的。

自动的资源回收，相较于手动的资源回收可能在性能上稍有差距，但是毫无疑问地增加了应用程序的健壮性。

---
#### 1.2 引用结构设计
仔细研究我们的**OpenCore.h**文件，你会发现它的引用列表是这样的：

```C++
#include "GfxCore.h"
#include "Macros.h"
#include "ResourceManager.h"
#include "SoundEffect.h"
#include "Timer.h"
#include "../Stage/StageController.h"
```

它几乎引用了我们引擎所包含的所有头文件，而当你去查看它包含的这些文件的引用列表，你几乎见不到他们互相之间进行引用。

这是一种设计观念，它毫无疑问是为了服务于最后的 **“高内聚、低耦合”** 的软件设计理念。

你甚至会发现，哪怕在需要调用 ResourceManager 中函数的 SoundEffectManager头文件之中，都没有去包含前者的头文件。这是为了防止循环引用，导致头文件过早引用以及代码错位。那么我们是如何在不引用头文件的情况下使得SFXManager能够调用ResourceManager的呢？

方法就是前向引用。
```C++
class ResourceManger; // 前向引用
class SoundEffectManager
{
    ···
    private:
    ResourceManger* res; // 指针引用
};
```
方法就如同上述类的实现方法，因为我们需要引用资源类的对象，那我们就在前方虚设一个同名类，这样即便不引用对应的头文件，我们的指针也不会报错。你可能会疑惑，我们在上面声明的类不是一个空类吗？这样不会造成 undefined class 的错误吗？

当然不会，正如同我们在头文件中写声明，而在CPP文件中写定义的分离式编程一般，我们也可以分离类的声明和定义。虽然在此处我们的**ResourceManager**类是一个空的声明，但是在编译的时候，编译器会自动匹配ResourceManager.h中的定义，这样一来，我们所使用的资源管理器类和头文件中定义的就指向了同一种类。

与此同时，我们成功的解耦了ResourceManger头和SoundEffectManager头文件。

而最终，我们将OpenEngine作为一个统一的引用入口，只要引用它就能引用所有的头文件，这也符合我们的模块化设计。