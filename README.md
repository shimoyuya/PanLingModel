# 🗡️ PanLing - 第三人称 ARPG 项目文档

## 📌 一、 整体架构概述 (Architecture Overview)

本项目基于 Unreal Engine 5.5 开发，采用 **C++ 与蓝图 (Blueprint) 协同** 的最佳实践开发模式。
核心架构思想为 **“组合优于继承 (Composition over Inheritance)”** 以及 **“事件驱动 (Event-Driven)”**：

* **组件化设计**：抛弃臃肿的角色基类，将生命值管理抽离为 `AttributeComponent`，将战斗逻辑抽离为 `CombatComponent`。玩家和敌人都可以通过“挂载”这些组件快速获得相应能力。
* **逻辑与表现分离**：核心数值、状态机、伤害计算、AI 寻路均由 C++ 底层处理；模型、动画、粒子特效和 UI 布局均交由蓝图处理，保证了极高的性能与可维护性。
* **接口解耦**：使用 `GameplayInterface` 处理所有环境交互（如开宝箱），角色无需关心交互对象的具体类型，极大地降低了代码耦合度。
* **精准战斗判定**：不依赖简单的范围伤害，而是结合动画系统的 `AnimNotifyState` 与武器的 **碰撞体 (BoxComponent)**，实现了“所见即所得”的精确物理挥砍判定，并自带防重复伤害机制。
* **事件驱动 UI**：抛弃了低效的 Event Tick 绑定 UI 方式，采用 C++ 委托 (Delegate) 广播机制。只有当血量真正发生变化时，UI 才会收到通知并更新性能开销极低。
* **AI 架构**：采用 UE 标准的 `AIController` + `AIPerception` (视觉感知) + `Blackboard` (黑板记忆) + `Behavior Tree` (行为树决策) 分层架构，逻辑清晰，易于扩展。

---

## 🎯 二、 已实现核心功能 (Features)

* **增强输入系统 (Enhanced Input)**：基于 UE5 最新输入框架，实现角色移动、视角控制与动作触发。
* **通用属性系统**：支持角色、敌人等一切拥有“生命值”的实体，统一的伤害与治疗结算机制。
* **接口化交互系统**：通过 C++ Interface 实现射线检测交互（如开宝箱），无需转换具体类 (Cast)，高度解耦。
* **精准动作战斗判定**：结合 Anim Notify State 与 Weapon Trace (武器重叠检测) 实现精准的挥剑判定，支持防重复扣血机制。
* **AI 行为树与感知**：基于 C++ 编写 AI 视觉感知模块，结合行为树 (Behavior Tree) 实现自动索敌、追踪与近战攻击逻辑。
* **UMG 数据绑定 UI**：C++ 底层控制的玩家血条 HUD，实时监听属性组件广播的血量变化。

---

## 💻 三、 C++ 核心类说明 (C++ Classes)

### 1. 角色与控制 (Character & Control)
* **`APanLingCharacter` (玩家角色类)**
    * **功能**：玩家控制的实体，负责处理摄像机视角、接收玩家的增强输入 (Enhanced Input)、执行移动、发起交互与攻击指令。
    * **核心函数**：
        * `Move()` / `Look()`：解析输入值并驱动角色移动和视角旋转。
        * `PrimaryInteract()`：向正前方发射射线 (Line Trace)，检测并触发实现了 `IGameplayInterface` 的物体。
        * `Attack()`：检查战斗组件状态，播放攻击动画蒙太奇 (Montage)。
        * `TakeDamage()`：重写引擎内置受击函数，将受到的伤害传递给自身的 `AttributeComponent` 进行扣血计算。

### 2. 组件系统 (Component System)
* **`UAttributeComponent` (属性组件)**
    * **功能**：通用的数值管理核心。负责存储当前生命值、最大生命值，处理伤害与治疗的加减逻辑，并向外广播属性变化事件。
    * **核心函数/事件**：
        * `ApplyHealthChange()`：核心逻辑。计算数值变化，限制边界 (Clamp)，并触发广播。
        * `IsAlive()` / `GetHealth()` / `GetMaxHealth()`：状态获取函数。
        * `OnHealthChanged`：多播委托 (Delegate)，当血量改变时通知绑定了该事件的对象（如 UI 或死亡判定逻辑）。

* **`UCombatComponent` (战斗组件)**
    * **功能**：管理武器的装备、攻击状态的锁（防止连续抽搐攻击），以及作为动画和武器之间的沟通桥梁。
    * **核心函数**：
        * `EquipWeapon()`：将生成的武器 Actor 附加到角色的指定骨骼插槽 (Socket) 上。
        * `StartWeaponTrace()` / `StopWeaponTrace()`：供动画通知调用，开启或关闭武器的碰撞判定。

### 3. 战斗与交互 (Combat & Interaction)
* **`APanLingWeapon` (武器基类)**
    * **功能**：代表游戏中的武器实体。包含武器模型和用于伤害判定的碰撞盒，负责过滤无效目标并向受击者发送伤害事件。
    * **核心函数**：
        * `SetCollisionEnabled()`：控制碰撞盒的开启与关闭。
        * `ClearHitActors()`：清空已记录的受击者数组，为下一次挥砍做准备。
        * `OnBoxOverlap()`：碰撞重叠回调。执行防误伤、防重复判定逻辑，并调用 `UGameplayStatics::ApplyDamage` 造成伤害。

* **`IGameplayInterface` (交互接口)**
    * **功能**：定义世界可交互物体的通用标准。
    * **核心函数**：
        * `Interact_Implementation()`：纯虚函数的蓝图本地化版本，由具体物体（如宝箱、NPC）自行实现交互后的表现。

* **`ASItemChest` (宝箱测试类)**
    * **功能**：实现了交互接口的具体 Actor。
    * **核心函数**：
        * `Interact_Implementation()`：接收到玩家射线后，执行翻转盖子的表现逻辑。

### 4. 人工智能 (Artificial Intelligence)
* **`APanLingEnemy` (敌人基类)**
    * **功能**：AI 控制的躯体。挂载了属性组件与战斗组件，处理受击表现与死亡逻辑。
    * **核心函数**：
        * `TakeDamage()`：接收玩家武器的伤害，转交给属性组件。
        * `OnHealthChanged()`：监听自身血量，当血量归零时禁用碰撞、停止移动，并设置定时销毁。
        * `Attack()`：供 AI 行为树调用，播放敌人的攻击动画。

* **`APanLingAIController` (AI 控制器)**
    * **功能**：敌人的“大脑”与“眼睛”。配置视觉感知，并在看到/丢失目标时更新黑板数据。
    * **核心函数**：
        * `OnPossess()`：附身敌人实体时，启动行为树 (Behavior Tree)。
        * `OnTargetPerceived()`：视觉感知回调。看到玩家时将其写入黑板的 `TargetActor` 键，丢失时清空。

* **`UBTTask_MeleeAttack` (自定义任务节点)**
    * **功能**：在行为树中执行近战攻击逻辑的 C++ 节点。
    * **核心函数**：
        * `ExecuteTask()`：获取拥有的 AI 实体，并调用其 `Attack()` 函数，随后返回任务成功状态。

### 5. 用户界面 (User Interface)
* **`UPanLingPlayerHUD` (玩家界面基类)**
    * **功能**：负责屏幕 UI 的逻辑处理。通过 `BindWidget` 强制绑定蓝图中的进度条，并监听玩家血量变化。
    * **核心函数**：
        * `InitializeHUD()`：接收玩家的属性组件，完成委托绑定并进行首次血条刷新。
        * `OnHealthChanged()`：事件回调。计算当前血量百分比，并调用 `SetPercent` 更新进度条 UI。

---

## 🎨 四、 蓝图类及资产说明 (Blueprint Classes & Assets)

虽然核心逻辑在 C++ 中，但表现层（模型、特效、UI布局、配置）均交由蓝图处理，实现了良好的开发解耦：

### 1. 角色与武器蓝图
* **`BP_PanLingCharacter`** (继承 `APanLingCharacter`)：配置了小白人模型 (SKM_Manny)、动画蓝图 (ABP_Manny)、增强输入上下文 (IMC_Default) 以及绑定的玩家 UI 类。
* **`BP_Enemy`** (继承 `APanLingEnemy`)：配置了敌人的模型、默认装备的武器 (`BP_Sword`)、AI 控制器 (`BP_EnemyAIController`) 以及攻击动画蒙太奇。
* **`BP_Sword`** (继承 `APanLingWeapon`)：配置了武器的静态网格体（剑的模型）以及用于物理判定的 Box Collision 的具体大小和相对位置。
* **`BP_Chest`** (继承 `ASItemChest`)：配置了宝箱底座与盖子的模型。

### 2. 动画与输入资产
* **`ANS_WeaponTrace`** (AnimNotifyState 蓝图)：挂载在攻击动画蒙太奇的帧轨道上。开始时调用 `StartWeaponTrace`，结束时调用 `StopWeaponTrace`，精确框定了武器的“杀伤时间段”。
* **Input Actions** (`IA_Move`, `IA_Look`, `IA_Interact`, `IA_Attack`)：定义了输入的数据类型（如 2D 轴、布尔按键），并在 `IMC_Default` 中映射了对应的键盘/鼠标按键。

### 3. AI 大脑资产
* **`BP_EnemyAIController`**：指定了敌人要运行的特定行为树资产。
* **`BB_Enemy`** (黑板 Blackboard)：AI 的内存，目前包含了一个 Object 类型的键 `TargetActor`，用于记忆玩家的位置。
* **`BT_Enemy`** (行为树 Behavior Tree)：定义了 AI 的决策树：`Sequence` -> [`寻找并移动到 TargetActor` -> `播放 Melee Attack 任务` -> `等待 (Wait) 2秒 CD`]。

### 4. UI 界面蓝图
* **`WBP_PlayerHUD`** (继承 `UPanLingPlayerHUD`)：界面布局蓝图。包含 Canvas Panel 和一个被严格命名为 `HealthProgressBar` 的 ProgressBar 组件，并配置了血条的颜色和屏幕位置。
