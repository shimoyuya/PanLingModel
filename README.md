# [cite_start]🗡️ PanLing - 第三人称 ARPG 项目文档 (Phase 1) [cite: 1]

## [cite_start]📌 1. 架构概览 (Architecture Overview) [cite: 2]

[cite_start]本项目采用 UE 5.5 + C++ 开发，遵循 “组合优于继承” (Composition over Inheritance) 的设计原则，放弃了沉重的 GAS 框架，选择轻量级、模块化的组件驱动架构。[cite: 3]

* [cite_start]**逻辑与表现分离**：核心数值、状态机、伤害计算、AI 寻路均由 C++ 底层处理；模型、动画、粒子特效和 UI 布局均交由 Blueprint (蓝图) 处理，保证了极高的性能与可维护性。[cite: 4]
* [cite_start]**事件驱动 (Event-Driven)**：广泛使用 UE 的委托机制 (Delegates) 实现 UI 更新和状态监听，彻底解耦了数据层与表现层。[cite: 5]

---

## [cite_start]🎯 2. 已实现核心功能 (Features) [cite: 6]

* [cite_start]**增强输入系统 (Enhanced Input)**：基于 UE5 最新输入框架，实现角色移动、视角控制与动作触发。[cite: 7]
* [cite_start]**通用属性系统**：支持角色、敌人等一切拥有“生命值”的实体，统一的伤害与治疗结算机制。[cite: 8]
* [cite_start]**接口化交互系统**：通过 C++ Interface 实现射线检测交互（如开宝箱），无需转换具体类 (Cast)，高度解耦。[cite: 9]
* [cite_start]**精准动作战斗判定**：结合 Anim Notify State 与 Weapon Trace (武器重叠检测) 实现精准的挥剑判定，支持防重复扣血机制。[cite: 10]
* [cite_start]**AI 行为树与感知**：基于 C++ 编写 AI 视觉感知模块，结合行为树 (Behavior Tree) 实现自动索敌、追踪与近战攻击逻辑。[cite: 11]
* [cite_start]**UMG 数据绑定 UI**：C++ 底层控制的玩家血条 HUD，实时监听属性组件广播的血量变化。[cite: 12]

---

## [cite_start]💻 3. C++ 核心类库说明 (C++ Classes) [cite: 13]

| 类名 (Class Name) | 继承自 (Parent) | 功能说明 (Description) |
| :--- | :--- | :--- |
| APanLingCharacter | ACharacter | [cite_start]玩家基类。负责绑定 Enhanced Input (移动/视角/交互/攻击)，在游戏开始时生成并装备武器，初始化并把自己的属性组件传递给 UI HUD 显示。同时重写了 TakeDamage 用于接收伤害。[cite: 14] |
| UAttributeComponent | UActorComponent | [cite_start]属性组件。系统的“心脏”。存储 Health 和 MaxHealth，提供 ApplyHealthChange 函数统一处理伤害与治疗。利用 OnHealthChanged 动态多播委托通知外界（如 UI 或死亡逻辑）血量变化。[cite: 14] |
| IGameplayInterface | UInterface | [cite_start]通用交互接口。定义了 Interact 纯虚函数。任何场景中可被玩家按 'E' 键互动的物品（如宝箱、NPC）只需继承此接口即可，解耦了玩家与互动物品的直接依赖。[cite: 14] |
| ASItemChest | AActor | [cite_start]交互物品测试类（宝箱）。继承了 IGameplayInterface 并重写了 Interact 逻辑，实现了被玩家射线击中后的开箱/关箱状态切换。[cite: 14] |
| APanLingWeapon | AActor | [cite_start]武器基类。包含模型和用于伤害检测的 UBoxComponent。内部维护一个 HitActors 数组防止单次挥剑对同一目标造成多次伤害。通过 ApplyDamage 将伤害传递给碰到的目标。[cite: 14] |
| UCombatComponent | UActorComponent | [cite_start]战斗组件。管理战斗状态。负责武器的挂载 (AttachToComponent)、记录当前是否在攻击 (bIsAttacking)，并提供开启/关闭武器碰撞判定的接口，供动画通知调用。[cite: 14] |
| APanLingEnemy | ACharacter | [cite_start]敌人基类。挂载了属性组件和战斗组件，重写了 TakeDamage 并绑定了血量清零时的死亡逻辑（关闭碰撞、停止移动、定时销毁）。提供 Attack 函数供 AI 行为树调用。[cite: 14] |
| APanLingAIController | AAIController | [cite_start]AI 视觉与大脑控制器。配置 UAIPerceptionComponent 实现视觉感知。当视野内出现目标时，自动将目标指针写入黑板 (Blackboard) 的 TargetActor 键值中。[cite: 14] |
| UBTTask_MeleeAttack | UBTTaskNode | [cite_start]自定义行为树任务节点。C++ 编写的 AI 逻辑节点。当 AI 靠近玩家时被调用，它会获取控制的 PanLingEnemy 躯体并触发其 Attack 函数，执行完毕后返回成功状态。[cite: 14] |
| UPanLingPlayerHUD | UUserWidget | [cite_start]UI 底层控制类。利用 BindWidget 宏强制绑定蓝图中的进度条。通过 InitializeHUD 接收属性组件，并将更新进度条的逻辑绑定到血量变化委托上。[cite: 14] |

---

## [cite_start]🎨 4. 蓝图类与资产说明 (Blueprint Classes) [cite: 15]

| 蓝图类名 (BP Name) | 继承自 (Parent) | 功能说明 (Description) |
| :--- | :--- | :--- |
| BP_PanLingCharacter | APanLingCharacter | [cite_start]配置小白人模型、动画蓝图。在 Details 面板中指定 Input Actions、默认生成的武器 BP_Sword、攻击播放的蒙太奇 AttackMontage，以及 UI WBP_PlayerHUD。[cite: 16] |
| BP_Enemy | APanLingEnemy | [cite_start]敌人的具体表现。配置红色/不同材质的模型以区分玩家。指定 BP_EnemyAIController 为其默认控制器，并配置敌人的武器和攻击蒙太奇。[cite: 16] |
| BP_Chest | ASItemChest | [cite_start]配置宝箱的底座 Mesh 和盖子 Mesh，用于在场景中测试交互。[cite: 16] |
| BP_Sword | APanLingWeapon | [cite_start]配置剑的模型，以及调节用于伤害判定的 Collision Box 的大小和位置，确保完全包裹剑刃。[cite: 16] |
| ANS_WeaponTrace | UAnimNotifyState | [cite_start]极其关键的动画通知状态。由于它无需复杂逻辑，直接用纯蓝图创建。在蒙太奇的特定帧调用玩家身上 Combat Component 的 StartWeaponTrace 和 StopWeaponTrace。[cite: 16] |
| BP_EnemyAIController | APanLingAIController | [cite_start]配置具体的行为树资产 BT_Enemy 并在附身时运行。[cite: 16] |
| BB_Enemy | BlackboardData | [cite_start]黑板记忆。存储 AI 需要的数据，目前拥有一个 TargetActor (Object/Actor) 键，用于记住玩家。[cite: 16] |
| BT_Enemy | BehaviorTree | [cite_start]AI 决策树。逻辑流为：Sequence -> MoveTo(TargetActor, 距离150) -> MeleeAttack(调用 C++ 节点) -> Wait(2秒)。[cite: 16] |
| WBP_PlayerHUD | UPanLingPlayerHUD | [cite_start]玩家界面。使用 Canvas Panel 布局，包含一个名为 HealthProgressBar 的进度条（名字严格匹配 C++ 绑定的变量），配置血条颜色和屏幕位置。[cite: 16] |
