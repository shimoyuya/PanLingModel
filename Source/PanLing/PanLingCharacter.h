// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "PanLingCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UAttributeComponent;
class APanLingWeapon;
class UInventoryComponent;
class UPanLingSkillComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UENUM(BlueprintType)
enum class EActionState : uint8
{
	Unoccupied, // 空闲状态，可以自由移动和做任何事
	Attacking,  // 攻击中，不能移动或闪避
	HitReact,   // 受击硬直中，不能做任何事
	Dodging,	// 翻滚中，有无敌帧
	Dead        // 死亡
};

UCLASS(config=Game)
class APanLingCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	//闪避
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DodgeAction;

protected:
	//属性组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAttributeComponent* AttributeComp;

	// 交互动作 (Input Action)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* InteractAction; // 记得去蓝图绑定这个

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCombatComponent* CombatComp;

public:
	APanLingCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	// 交互的具体实现函数
	void PrimaryInteract();
			
	// 玩家的 HUD 蓝图类 (用于在编辑器中指定 WBP_PlayerHUD)
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UPanLingPlayerHUD> PlayerHUDClass;

	// 保存创建出来的 HUD 实例
	UPROPERTY()
	UPanLingPlayerHUD* PlayerHUDInstance;

	// AI 刺激源组件（让 AI 能稳定听到/看到玩家）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class UAIPerceptionStimuliSourceComponent* StimuliSourceComp;

protected:

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	// --- 武器系统 ---

	// 在编辑器里指定我们要生成哪种武器 (例如 BP_Sword)
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<APanLingWeapon> WeaponClass;

	// 挂载武器的插槽名字 (默认设为 "WeaponSocket")
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FName WeaponSocketName;

	// 攻击输入
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* AttackAction;

	// 实现攻击函数
	void Attack();

public:
	// 获取当前武器 (未来做伤害判定时可能需要)
	UFUNCTION(BlueprintCallable, Category = "Combat")
	APanLingWeapon* GetEquippedWeapon() const;

	// 攻击动画播放结束的回调 (可选，用于重置连招状态)
	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	// 重写受击函数
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// 当前状态
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State")
	EActionState ActionState = EActionState::Unoccupied;

public:
	// 每帧更新函数，我们将在这里处理镜头平滑跟随
	virtual void Tick(float DeltaTime) override;

protected:
	/*========================================================================
	 * 目标锁定系统 (Target Lock-on System)
	 *======================================================================*/

	 // 锁定按键输入
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LockOnAction;

	// 当前锁定的目标
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|LockOn")
	AActor* LockedTarget;

	// 是否正在锁定中
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|LockOn")
	bool bIsLockedOn;

	// 搜索敌人的最大半径
	UPROPERTY(EditDefaultsOnly, Category = "Combat|LockOn")
	float LockOnRadius = 1500.f;

	// 镜头跟随敌人的平滑速度
	UPROPERTY(EditDefaultsOnly, Category = "Combat|LockOn")
	float LockOnInterpSpeed = 5.0f;

	// 触发锁定的主逻辑
	void ToggleLockOn();

	// 寻找最近的最佳目标
	void FindLockOnTarget();

	// 清除锁定状态
	void ClearLockOn();


	//闪避无敌帧系统
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* DodgeMontage;

	// 闪避消耗的精力值
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float DodgeStaminaCost = 25.0f;

	// 标记是否处于无敌帧
	bool bIsInvincible = false;

public:
	// 绑定到输入的闪避函数
	void Dodge();

	
	/*背包系统*/
protected:
	// 背包组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInventoryComponent* InventoryComp;

	// 用于空手装备时，生成的武器基类
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<class APanLingWeapon> BaseWeaponClass;

	// 用于丢弃物品时，在地上生成的拾取物基类
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<class APickupBase> PickupClass;

	// 记录当前装备武器的 ItemID，方便我们在替换武器时卸载旧的属性加成
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory|Equipment")
	FName CurrentEquippedWeaponID;

public:
	// 提供一个获取背包组件的接口
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UInventoryComponent* GetInventoryComponent() const { return InventoryComp; }

	// 供 UI 调用的使用物品接口
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UseItem(const FPanLingItemInfo& ItemInfo);

	// 丢弃物品接口
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void DropItem(const FPanLingItemInfo& ItemInfo, int32 InventoryIndex);

	/* 技能系统 */
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPanLingSkillComponent* SkillComp;

	// 技能 1 的输入
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* Skill1Action;

	void UseSkill1();
};

