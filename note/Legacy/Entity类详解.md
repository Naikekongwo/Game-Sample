# Entity 类详解

> 刘骐源 二零二六年五月十四日

## 概述

`Entity` 是 OpenCore 引擎中游戏实体的运行时载体，组合了**物理模拟**、**精灵表渲染**、**碰撞检测**、**动画系统**四个子系统。所有可移动、可交互的角色（玩家、NPC、怪物）均由此类驱动。

---

## 一、类结构与成员

### 1.1 组合关系

```
Entity
├── PhysicalProperties pProperties   ← 物理数据（位置/速度/加速度/重力）
├── unique_ptr<Mob> renderer         ← SDL 渲染代理（精灵表纹理）
├── EntityInfo info                  ← 配置元数据（贴图布局/动画序列）
├── BackPtr backpack                 ← 背包系统引用
├── shared_ptr<float> m_healthPercent ← 血量百分比（shared_ptr 便于外部 UI 绑定）
└── EntityStatus status              ← 生命周期状态机
```

### 1.2 成员一览

| 成员 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `pProperties` | `PhysicalProperties` | — | 物理组件：位置 (x,y,z)、速度、加速度、重力、朝向、瓦片尺寸 |
| `renderer` | `unique_ptr<Mob>` | `nullptr` | 惰性创建，持有一个 `Texture` 精灵表 |
| `info` | `EntityInfo` | — | 实体配置元数据 |
| `backpack` | `BackPtr` | — | 背包引用 |
| `status` | `EntityStatus` | `Registered` | 状态：Registered → Drawable → Ready |
| `drawable` | `bool` | `false` | 是否可渲染，`enableDrawer()` 开启 |
| `widthRelative` | `float` | `1.0f` | 世界 X 坐标→屏幕百分比换算因子 |
| `heightRelative` | `float` | `1.0f` | 世界 Y 坐标→屏幕百分比换算因子 |
| `lastTime` | `float` | `0.0f` | 上一帧时间戳，用于 deltaTime 计算 |
| `m_animTimer` | `float` | `0.0f` | 动画帧计时器，秒为单位累加 |
| `m_animFPS` | `float` | `6.0f` | 动画播放速率 |
| `m_currentAnimIndex` | `uint8_t` | `0` | 当前播放的动画序列索引 |
| `m_gridCols` | `uint8_t` | `4` | 精灵表网格列数（行→线性索引转换用） |
| `m_healthPercent` | `shared_ptr<float>` | — | 血量百分比，外部可订阅 |
| `maxHealth` | `uint8_t` | `255` | 最大血量（预留） |

---

## 二、EntityInfo 配置结构

```cpp
struct AnimFrame {
    uint8_t col, row;   // 帧在精灵表网格中的左上角坐标
    uint8_t tileW, tileH; // 本帧瓦片尺寸，0=沿用实体默认
};

struct AnimSequence {
    std::vector<AnimFrame> frames; // 一个完整动作循环的帧序列
};

struct EntityInfo {
    short EntityTypeID;          // 实体类型 ID（0=无效）
    short TextureID;             // 精灵表纹理资源 ID
    uint8_t backpackCapacity;    // 背包容量
    float widthFactor;           // 实体占据的瓦片宽度
    float heightFactor;          // 实体占据的瓦片高度
    uint8_t frameCount;          // 每方向动画帧数（公式法回退用）
    uint8_t defaultCol;          // 静态占位贴图列（无动画实体）
    uint8_t defaultRow;          // 静态占位贴图行（无动画实体）
    std::vector<AnimSequence> animations; // 显式动画序列
};
```

### 动画约定

| animations 索引 | 触发条件 | 含义 |
|----------------|---------|------|
| 0 | 静止（speed=0） | idle |
| 1 | 移动 + Down | walk_down |
| 2 | 移动 + Left | walk_left |
| 3 | 移动 + Right | walk_right |
| 4 | 移动 + Up | walk_up |
| 5+ | `setAnimIndex(n)` 手动切换 | attack / death / skill |

### 三种配置模式

| 模式 | animations | frameCount | defaultCol/Row | 适用 |
|------|-----------|------------|----------------|------|
| 显式动画 | 非空 | 忽略 | 忽略 | 需要灵活贴图映射的角色 |
| 公式动画 | 空 | >1 | (0,0) | 标准 4×4 精灵表，4 方向 × N 帧 （这个主要是针对两个人物的快捷方法）|
| 静态占位 | 空 | 1 | (x,y) | 道具、装饰物等无动画实体 |

---

## 三、生命周期

```
Configure(info)  →  createRenderer()  →  [每帧循环] onUpdate() → Draw()
```

### 3.1 状态机

```
Registered ──enableDrawer()──→ Drawable ──Configure()──→ Ready
```

`Ready()` 返回 `status >= Drawable`，表示实体已可渲染。

---

## 四、方法详解

### 4.1 `Configure(const EntityInfo& eInfo)`

**调用时机**：实体创建后一次调用。

**工作流程**：
1. 拷贝 EntityInfo 到成员 `info`
2. 通过 WorldController 创建背包
3. 初始化血量百分比为 1.0（满血）
4. 将 `widthFactor` / `heightFactor` 同步到 `PhysicalProperties` 的 `tileWidth` / `tileHeight`（碰撞检测依赖）
5. 状态设为 `Ready`

**关键点**：瓦片尺寸从 EntityInfo 到 PhysicalProperties 的传递是碰撞检测正确性的前提。

---

### 4.2 `createRenderer()`

**调用时机**：首次 `Draw()` 时惰性创建，或 `renderer` 为 nullptr 时重建。

**工作流程**：

1. **计算精灵表网格**
   ```
   gridCols = widthFactor × frameCount
   gridRows = heightFactor × 4       // 4 个方向
   ```

2. **创建 Mob 渲染器**
   - 传入 `TextureID` + `gridCols` + `gridRows`（替代旧版硬编码 4×4）
   - Mob 内部调用 `Texture(gridCols, gridRows, sdlTexture)` 划分网格

3. **设置锚点**：`AnchorPoint::BottomCenter`（底部中心），确保不同高度实体脚底对齐

4. **计算换算因子**
   ```
   widthRelative  = 1.0 / renderWidth
   heightRelative = 1.0 / renderHeight
   ```
   用于世界坐标 → 屏幕百分比坐标的转换

5. **传递瓦片尺寸**：`renderer->setTileSize(tileWidth, tileHeight)`，Mob 以此调用 `getSubRect(index, cols, rows)` 提取多瓦片块

6. **设置缩放**：`Scale(widthFactor × widthRelative, 0)`，高度 0 触发纹理宽高比自动推算

---

### 4.3 `onUpdate(float totalTime)`

**调用时机**：每帧。

**工作流程**（按顺序）：

```
┌─ 1. 计算 deltaTime = totalTime - lastTime
├─ 2. 保存旧位置 previousPosition
├─ 3. pProperties.onUpdate(totalTime)  → 物理刷新
├─ 4. canMoveTo(newPosition)           → 碰撞后检
│       不通过 → 回退位置 + 清零速度
├─ 5. 动画计时
│       静止 → timer=0, animIndex=0
│       移动 → computeAnimIndex() 切序列, timer 累加
├─ 6. 血量衰减
└─ 7. lastTime = totalTime
```

**动画计时逻辑**：

```
Vec3 speed = pProperties.getSpeed()
isMoving = |speed.x| > 0.01 || |speed.y| > 0.01

if !isMoving:
    m_animTimer = 0           // 定格 idle 首帧
    m_currentAnimIndex = 0
else:
    newIndex = computeAnimIndex()
    if newIndex != m_currentAnimIndex:
        m_animTimer = 0       // 切换序列时重播
        m_currentAnimIndex = newIndex
    m_animTimer += deltaTime  // 推进帧
```

**血量衰减**：每秒减少 5%，归零后重置为满血（当前为临时逻辑）。

---

### 4.4 `Draw(const Vec3& absPos)` — 绝对坐标绘制

**调用时机**：每帧（绝对屏幕坐标版本，由外部计算好坐标传入）。

**工作流程**：

1. 检查 `drawable` 标志
2. 确认 `renderer` 存在（惰性创建）
3. **帧选取**（见下方 4.6）
4. 设置屏幕位置 + 透明度
5. 调用 `renderer->Draw()` → `Mob::Draw()` → `GFX.Draw()`

---

### 4.5 `Draw(float cameraX, float cameraY)` — 相机相对绘制

**调用时机**：每帧（带视口裁剪的版本）。

**额外步骤**：
1. 计算渲染视口范围
   ```
   renderWidth  = (renderWidth  - 2) / 2 + 1
   renderHeight = (renderHeight - 1) / 2 + 1
   ```
2. **视口裁剪**：超出范围的实体直接 return，不绘制
3. 相机坐标换算：
   ```
   screenX = 0.5 + (Position.x - cameraX) × widthRelative
   screenY = 0.5 + (Position.y - cameraY) × heightRelative
   ```
4. 帧选取 + 绘制（同 4.4）

**优化**：`drawable=false` 时主动 `renderer.reset()` 释放渲染资源。

---

### 4.6 帧选取逻辑（两个 Draw 共用）

```
┌─ animations 非空？
│   YES → 路径 A：显式动画
│   NO  → 路径 B：公式回退
```

**路径 A — 显式动画**：
```cpp
seq = info.animations[m_currentAnimIndex]
f = (m_animTimer × m_animFPS) % seq.frames.size()
frame = seq.frames[f]
frameIndex = frame.row × m_gridCols + frame.col   // 网格坐标→线性索引

// 逐帧独立瓦片尺寸
if frame.tileW > 0: 更新 renderer tileWidth
if frame.tileH > 0: 更新 renderer tileHeight
```

**路径 B — 公式回退**：
```cpp
tilesPerDir = tileW × tileH × frameCount
base = directionBaseIndex(direction, tilesPerDir)
animFrame = (m_animTimer × m_animFPS) % frameCount
frameIndex = defaultRow × m_gridCols + defaultCol + base + animFrame
```

最终 `frameIndex` 写入 `renderer->getVisualState()->frameIndex`，由 `Mob::Draw()` 使用：
```cpp
// 单瓦片实体
getSubRect(frameIndex)         → 提取 1×1 格子
// 多瓦片实体
getSubRect(frameIndex, tileW, tileH) → 提取 N×M 格子块
```

---

### 4.7 `canMoveTo(const Vec3& pos) const`

**功能**：检测实体在目标位置是否与阻挡瓦片碰撞。

**工作流程**：

```
计算锚点格坐标:
  baseX = floor(pos.x)
  baseY = floor(pos.y)

遍历 tileW × tileH 格:
  gx = baseX + dx
  gy = baseY - dy          ← 锚点在底部，格子从底向上数
  block = WorldController::queryBlockInfo(gx, gy)
  if block 存在 && block.Access == 0:
      return false          ← 0=阻挡，不可通行

return true                 ← 全部可通行
```

**注意**：当前约定 `Access == 0` 为阻挡，非 0 可通行。碰撞是**硬碰撞**——不通过则直接回退到上一帧位置，不做推离。

---

### 4.8 `directionBaseIndex(Direction dir, int tilesPerDirection)`

**功能**：将方向枚举映射为精灵表中的行偏移乘数。

| Direction | 乘数 | 公式法返回 | 说明 |
|-----------|------|-----------|------|
| Down | 0 | 0 | 第 0 行块 |
| Left | 1 | tilesPerDirection | 第 1 行块 |
| Right | 2 | tilesPerDirection × 2 | 第 2 行块 |
| Up | 3 | tilesPerDirection × 3 | 第 3 行块 |

仅在公式回退路径（`animations` 为空）中使用。

---

### 4.9 `computeAnimIndex() const`

**功能**：根据当前方向计算移动时应播放的动画序列索引。

**前置条件**：仅在 `isMoving=true` 时由 `onUpdate()` 调用。

```cpp
if animations.empty(): return 0           // 回退模式

switch direction:
    Down  → 1    // walk_down 序列
    Left  → 2    // walk_left 序列
    Right → 3    // walk_right 序列
    Up    → 4    // walk_up 序列
    default → 0
```

---

### 4.10 `setAnimIndex(uint8_t)` / `getAnimIndex()`

**功能**：外部直接切换动画序列（用于攻击/死亡等非方向驱动动作）。

调用后 `onUpdate()` 的下一帧检测到索引变化会自动重置计时器从头播放。

---

### 4.11 `setDesiredVelocity(const Vec3&)`

**功能**：设置期望水平速度。输入系统通过此接口将键盘输入转为物理加速度目标。

内部委托：`pProperties.setDesiredVelocity(vel)`

---

### 4.12 `configMoveParams(float maxAccel, float gain)`

**功能**：调节移动手感参数。

- `maxAccel`：最大加速度（单位/s²），数值越大响应越灵敏（12~20）
- `gain`：趋近增益，接近目标速度时的柔和度（8~15）

---

## 五、完整数据流

```
输入系统                           Entity                              SDL/GPU
───────                          ──────────                           ───────
WASD 按键
  → MapExplorer.handlEvents()
  → m_wrdController->regMovement()
  → entity->setDesiredVelocity(x,y,0)
                                    │
                              onUpdate(totalTime)
                                    │
                              pProperties.onUpdate()
                              ├─ processAxis(Speed.x, Position.x)
                              ├─ processAxis(Speed.y, Position.y)
                              └─ parseVerticalMovement(Speed.z, Position.z)
                                    │
                              canMoveTo(newPosition)
                              ├─ 通过 → 保持新位置
                              └─ 不通过 → 回退旧位置, 清零速度
                                    │
                              动画计时
                              ├─ isMoving? → computeAnimIndex()
                              │   ├─ 序列切换 → timer=0
                              │   └─ 序列保持 → timer+=dt
                              └─ 静止 → timer=0, animIndex=0
                                    │
                              Draw(cameraX, cameraY)
                              ├─ 视口裁剪检查
                              ├─ 相机坐标换算 → setPosition
                              └─ 帧选取
                                  ├─ 显式序列 → row*m_gridCols+col
                                  └─ 公式回退 → offset+dirBase+animFrame
                                    │
                              renderer->Draw()
                              └─ Mob::Draw()
                                  ├─ getLogicalBounds() → dstRect
                                  ├─ getSubRect(frameIndex, tileW, tileH) → srcRect
                                  └─ GFX.Draw(texture, &srcRect, &dstRect)
```

---

## 六、关键约束与注意事项

1. **瓦片尺寸一致性**：`EntityInfo.widthFactor/heightFactor` 在 `Configure()` 传入 `PhysicalProperties`，后续碰撞和渲染均依赖此值
2. **锚点约定**：实体锚点为底部中心，`getOccupiedTiles()` 的 `baseY - dy` 从底向上数格
3. **网格列数存储**：`m_gridCols` 在 `createRenderer()` 计算并存储，帧坐标 `(col, row)` 转线性索引时必需
4. **动画优先级**：显式序列（`animations` 非空）> 公式回退（`animations` 为空）
5. **碰撞轴**：使用 XY 平面（Position.x, Position.y），Z 轴为垂直高度仅用于重力/跳跃
6. **硬碰撞**：不通过直接回退，无推离滑动，不适合需要沿墙滑动的场景
