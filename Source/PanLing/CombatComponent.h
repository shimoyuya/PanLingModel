// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class APanLingWeapon;
class APanLingCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PANLING_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// 缓存拥有这个组件的角色
	UPROPERTY()
	ACharacter* CharacterOwner;

	// 当前装备的武器
	UPROPERTY()
	APanLingWeapon* EquippedWeapon;

	// 标记当前是否正在攻击（用于连招和防止重复播放）
	bool bIsAttacking;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 装备武器
	void EquipWeapon(APanLingWeapon* WeaponToEquip);

	// 获取当前武器
	UFUNCTION(BlueprintCallable, Category = "Combat")
	APanLingWeapon* GetEquippedWeapon() const;

	// 开始攻击判定 (由动画通知调用)
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartWeaponTrace();

	// 结束攻击判定 (由动画通知调用)
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StopWeaponTrace();

	// 设置攻击状态
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetAttacking(bool bAttacking) { bIsAttacking = bAttacking; }
	bool IsAttacking() const { return bIsAttacking; }

public:
	// 玩家调用的主要攻击接口
	void RequestAttack();

	// 由动画通知(AnimNotify)调用的回调函数
	void CheckCombo();        // 检查是否进入下一段连招
	void ResetCombatState();  // 重置所有战斗状态

	// 供 AnimNotify 调用的位移函数，传入冲刺力度
	void AttackDash(float DashForce);

protected:
	// 配置参数：你要播放的连招蒙太奇
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Animation")
	class UAnimMontage* ComboMontage;

	//微锁定配置参数
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Targeting")
	float TargetingRadius = 400.0f; // 索敌半径

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Targeting")
	float TargetingAngle = 60.0f; // 索敌角度（正前方多少度以内）

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Targeting")
	float RotationInterpSpeed = 15.0f; // 转身插值速度

private:
	// 状态变量
	bool bSaveAttack;    // 是否缓存了下一次攻击的输入
	int32 ComboCount;    // 当前处于第几段攻击 (1, 2, 3...)

	// 内部执行连招的函数
	void PerformAttack();

	//微锁定状态
	UPROPERTY()
	AActor * CurrentTarget; // 当前锁定的敌人
	bool bIsRotating;      // 是否正在执行转身

	// 核心寻敌算法
	void FindNearestTarget();
};
