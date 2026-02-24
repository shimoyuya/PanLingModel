// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponDataAsset.generated.h"

class UAnimMontage;
class UNiagaraSystem;
class USoundBase;
class UCameraShakeBase;

/**
 * 武器数据资产：用于集中管理武器的所有静态数据
 */
UCLASS(BlueprintType)
class PANLING_API UWeaponDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// 武器的静态模型
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Visuals")
	UStaticMesh* WeaponMesh;

	// 基础伤害
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stats")
	float BaseDamage = 20.0f;

	// 攻击消耗的精力值
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Stats")
	float AttackStaminaCost = 10.0f;

	// 该武器专属的攻击蒙太奇 (比如大剑和匕首的挥舞动作是不同的)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Animation")
	UAnimMontage* AttackMontage;

	// 击中特效
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Effects")
	UNiagaraSystem* HitVFX;

	// 击中音效
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Effects")
	USoundBase* HitSound;

	// --- 战斗手感 (Game Feel) ---
	// 击中时的屏幕震动类
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Feel")
	TSubclassOf<UCameraShakeBase> HitCameraShake;

	// 顿帧：现实中的持续时间 (秒)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Feel")
	float HitStopDuration = 0.05f;

	// 顿帧：游戏时间的流逝速度 (例如0.1代表游戏速度降为原来的10%)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Feel")
	float HitStopTimeDilation = 0.1f;

};
