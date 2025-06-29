# Game-Sample
使用C++/SDL2开发的Windows游戏样带。
> WARNING: 本地使用MinGW编译时，请使用如下命令：“g++ *.cpp -o Game-Sample.exe -lmingw32 -lSDL2main -lSDL2 -lSDL2_image”

\* 该命令的内容是在将目录下所有源码编译同时引用mingw和SDL库，倘若不使用该命令，可能会出现 **undefined reference to `WinMain`** 等错误。

* SDL2 ver. **2.30.7**
* MinGW ver. **14.2.0**

\* 确保你在编译时使用的是相同的版本，以保证编译的稳定性。
### **Period One** ：初创阶段
* C++ With SDL2 
* 2025.5 —> 2025.6

游戏名称已经确定为 **`ECLIPSEA / 无水之境`**。 <br>
引擎名称已经确定为 **`OpenCore Engine`**。

第一阶段目标如下:
* **场景系统完善**
* **地图系统（除地图编辑器外）完善**
* **UI控件系统初步**
* **LANG文本/语言切换系统的预置**

第一阶段计划至此结束。