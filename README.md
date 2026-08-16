# Eclipsea

基于新 OpenCore 引擎（26.1）重构的原 Game-Project 游戏项目。

> 本项目由旧版本 `Game-Project/Game-Sample` 移植而来。原项目内嵌旧版 OpenCore
> （SDL2 / C++26），本仓库基于新版 OpenCore 子模块（SDL3 / C++23）重建。

## 结构

```
Eclipsea/
├── CMakeLists.txt          # 构建配置（C++23 / SDL3 / OpenCore 子模块）
├── modules/OpenCore        # OpenCore 引擎（git submodule）
├── include/Eclipsea/       # 项目公共头文件
│   ├── Eclipsea.hpp        # 项目资源 ID 与总入口
│   ├── Core/               # 项目级核心（音频、设置）
│   ├── Gameplay/           # 项目级玩法系统（实体/背包/地图/物理，原引擎已移除）
│   └── Stage/              # 场景
├── src/
│   ├── Eclipsea.cpp        # 游戏启动与资源注册
│   ├── Core/               # 音频模块（SDL3_mixer）、设置模块
│   ├── Gameplay/           # 玩法系统实现
│   └── Stage/              # 场景实现
├── project/Eclipsea.cpp    # main() 入口
└── assets/                 # 游戏资源
```

## 构建

```bash
cmake -S . -B build
cmake --build build
```

Windows 下输出到 `build/Eclipsea.exe`，资源自动复制到输出目录。

## 与新引擎的差异说明

新版 OpenCore 相比旧版（Game-Sample 内嵌版）：

- **移除**了玩法系统（实体/背包物品/地图/世界控制器/物理碰撞）与音频模块
- 资源管理从「数字 ID + JSON 清单」改为「名称 + PackageManager 资源包系统」
- 场景构造不再传 `(Timer*, StageManager*)`，由 `StageManager::configure()` 注入
- 事件从 `handlEvents(SDL_Event*)` 改为 `parseEvents(Event*)`
- SDL2 → SDL3，C++26 → C++23

因此本项目的玩法系统（`include/Eclipsea/Gameplay`、`src/Gameplay`）与音频模块
（`include/Eclipsea/Core`、`src/Core`）均为**项目级重新实现**，不修改引擎本身。

详见 [`docs/移植分析.md`](docs/移植分析.md)。
