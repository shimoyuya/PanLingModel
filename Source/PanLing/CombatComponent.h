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
};
