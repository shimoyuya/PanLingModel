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

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

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

	// 具体的攻击动作 (Montage)，我们下一步会用到
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* AttackMontage;

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
};

