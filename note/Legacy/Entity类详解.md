# Entity 类详解

> 刘骐源 二零二六年五月十六日

## 概述

`Entity` 是 OpenCore 引擎中游戏实体的运行时载体，当前为**纯物理+渲染载体**，组合了**物理模拟**、**精灵表渲染**、**碰撞检测**三个子系统。动画系统已从 Entity 中解耦，将在后续作为独立模块重新设计。

---

## 一、类结构与成员

### 1.1 组合关系

```
Entity
├── PhysicalProperties pProperties   ← 物理数据（位置/速度/加速度/重力/瓦片尺寸）
├── unique_ptr<Mob> renderer         ← SDL 渲染代理（精灵表纹理）
├── EntityInfo info                  ← 配置元数据（纹理资产 + 背包 + 尺寸）
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
| `m_baseFrame` | `uint16_t` | `0` | 精灵表中实体纹理的起始线性索引 (`originRow*gridCols+originCol`) |
| `lastTime` | `float` | `0.0f` | 上一帧时间戳，用于 deltaTime 计算 |
| `m_healthPercent` | `shared_ptr<float>` | — | 血量百分比，外部可订阅 |
| `maxHealth` | `uint8_t` | `255` | 最大血量（预留） |

---

## 二、EntityInfo 配置结构

```cpp
struct TextureAsset {
    short textureID = 0;       // 纹理资源 ID
    uint8_t gridCols = 4;      // 精灵表总列数
    uint8_t gridRows = 4;      // 精灵表总行数
    uint8_t originCol = 0;     // 该实体纹理在精灵表中的起始列
    uint8_t originRow = 0;     // 该实体纹理在精灵表中的起始行
};

struct EntityInfo {
    short EntityTypeID = 0;         // 实体类型 ID（0=无效）
    TextureAsset texture;           // 纹理资产（ID + 网格维度 + 起始偏移）
    uint8_t backpackCapacity = 0;   // 背包容量
    float widthFactor = 1.0f;       // 实体占据的瓦片宽度
    float heightFactor = 1.0f;      // 实体占据的瓦片高度
};
```

### 纹理定位方式

`originCol` / `originRow` 指定该实体纹理在精灵表中的左上角格子坐标。`createRenderer()` 据此计算基准线性帧索引：

```
m_baseFrame = originRow × gridCols + originCol
```

渲染时 `VState->frameIndex` 被设为此基准值，Mob 通过 `getSubRect(frameIndex, ...)` 提取纹理矩形：

- **单瓦片实体** (`tileWidth==1 && tileHeight==1`)：`getSubRect(frameIndex)` → 提取 1×1 格子
- **多瓦片实体** (`tileWidth>1 || tileHeight>1`)：`getSubRect(frameIndex, tileWidth, tileHeight)` → 提取 N×M 格子块

这样同一张精灵表可存放多个实体的纹理，例如：

```
精灵表 (4×4):              各实体配置:
┌───┬───┬───┬───┐
│ A₀│ A₁│ B₀│ B₁│   实体 A: originCol=0, originRow=0
├───┼───┼───┼───┤   实体 B: originCol=2, originRow=0
│ A₂│ A₃│ B₂│ B₃│
├───┼───┼───┼───┤
│ B₄│ B₅│ C₀│ C₁│   实体 B 多帧: originCol=0, originRow=2, tileW=2, tileH=2
├───┼───┼───┼───┤
│ B₆│ B₇│ C₂│ C₃│   实体 C: originCol=2, originRow=2
└───┴───┴───┴───┘
```

动画帧的选取逻辑（帧索引计算、方向映射、帧切换）将作为独立模块在后续版本中实现，不再内置于 Entity 类。

### 构造示例

```cpp
// 1×1 角色实体，纹理从精灵表 (0,0) 开始
EntityInfo player{PLAYER_ENTITYTYPE, {player_texture, 4, 4, 0, 0}, 8, 1.0f};

// 3×3 大型实体，纹理从精灵表 (0,0) 开始
EntityInfo purifier{100, {2044, 4, 4, 0, 0}, 4, 3.0f, 3.0f};

// 实体纹理从精灵表 (2,2) 开始
EntityInfo npc{50, {shared_tex, 8, 8, 2, 2}, 4, 1.0f, 1.0f};
```

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

1. **创建 Mob 渲染器**
   - 从 `info.texture`（TextureAsset）读取 `textureID`、`gridCols`、`gridRows`
   - Mob 内部调用 `Texture(gridCols, gridRows, sdlTexture)` 划分网格

2. **设置锚点**：`AnchorPoint::BottomCenter`（底部中心），确保不同高度实体脚底对齐

3. **计算换算因子**
   ```
   widthRelative  = 1.0 / renderWidth
   heightRelative = 1.0 / renderHeight
   ```
   用于世界坐标 → 屏幕百分比坐标的转换

4. **传递瓦片尺寸**：`renderer->setTileSize(tileWidth, tileHeight)`，Mob 以此调用 `getSubRect(index, cols, rows)` 提取多瓦片块

5. **计算基准帧索引**：`m_baseFrame = originRow × gridCols + originCol`，绘制时写入 `VState->frameIndex`

6. **设置缩放**：`Scale(widthFactor × widthRelative, 0)`，高度 0 触发纹理宽高比自动推算

---

### 4.3 `onUpdate(float totalTime)`

**调用时机**：每帧，由 `WorldController::onUpdate()` 统一调度。

**工作流程**（按顺序）：

```
┌─ 1. 计算 deltaTime = totalTime - lastTime
├─ 2. 保存旧位置 previousPosition
├─ 3. pProperties.onUpdate(totalTime)  → 物理刷新
├─ 4. canMoveTo(newPosition)           → 碰撞后检
│       不通过 → 回退位置 + 清零速度
├─ 5. 血量衰减 (每秒 -5%, 归零重置满血)
└─ 6. lastTime = totalTime
```

**血量衰减**：每秒减少 5%，归零后重置为满血（当前为临时逻辑）。

---

### 4.4 `Draw(const Vec3& absPos)` — 绝对坐标绘制

**调用时机**：每帧（绝对屏幕坐标版本，由 MapExplorer 计算好坐标传入）。

**工作流程**：

1. 检查 `drawable` 标志
2. 确认 `renderer` 存在（惰性创建）
3. 设置屏幕位置 + 透明度
4. 将 `m_baseFrame` 写入 `renderer->getVisualState()->frameIndex`
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
4. 写入 `frameIndex = m_baseFrame` + 设置透明度 + 绘制（同 4.4）

**优化**：`drawable=false` 时主动 `renderer.reset()` 释放渲染资源。

---

### 4.6 `canMoveTo(const Vec3& pos) const`

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

### 4.7 `setDesiredVelocity(const Vec3&)`

**功能**：设置期望水平速度。输入系统通过此接口将键盘输入转为物理加速度目标。

内部委托：`pProperties.setDesiredVelocity(vel)`

---

### 4.8 `configMoveParams(float maxAccel, float gain)`

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
                              血量衰减
                                    │
                              Draw(absPos)
                              ├─ setPosition(screenX, screenY)
                              └─ setTransparency(1.0f)
                                    │
                              renderer->Draw()
                              └─ Mob::Draw()
                                  ├─ getLogicalBounds() → dstRect
                                  ├─ getSubRect(frameIndex [, tileW, tileH]) → srcRect
                                  └─ GFX.Draw(texture, &srcRect, &dstRect)
```

---

## 六、关键约束与注意事项

1. **瓦片尺寸一致性**：`EntityInfo.widthFactor/heightFactor` 在 `Configure()` 传入 `PhysicalProperties`，后续碰撞和渲染均依赖此值
2. **锚点约定**：实体锚点为底部中心，`getOccupiedTiles()` 的 `baseY - dy` 从底向上数格
3. **纹理网格独立**：精灵表网格由 `TextureAsset::gridCols/gridRows` 定义，与实体瓦片尺寸解耦
4. **碰撞轴**：使用 XY 平面（Position.x, Position.y），Z 轴为垂直高度仅用于重力/跳跃
5. **硬碰撞**：不通过直接回退，无推离滑动，不适合需要沿墙滑动的场景
6. **动画分离**：Entity 不再持有任何动画状态，帧索引的选取逻辑将由独立动画模块负责
