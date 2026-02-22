// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"
#include "PanLingWeapon.generated.h"

class UBoxComponent;

UCLASS()
class PANLING_API APanLingWeapon : public AActor
{
	GENERATED_BODY()
	
protected:
	// 武器的模型 (可能是骨骼模型也可能是静态模型，这里用 StaticMesh 简单演示)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	// 武器的名字 (可选，用于 UI 显示)
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponName;

	// 基础伤害值
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float BaseDamage = 20.0f;

	// 记录一次攻击中已经击中过的 Actor，防止多帧重复扣血
	UPROPERTY()
	TArray<AActor*> HitActors;

	// 标记当前是否正在挥刀
	bool bIsTracing;

	// 击中时的粒子特效
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Effects")
	UNiagaraSystem* HitVFX;

	// 击中时的音效
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Effects")
	USoundBase* HitSound;

public:	
	// Sets default values for this actor's properties
	APanLingWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// --- 全新精准轨迹检测接口 ---
	void StartWeaponTrace();
	void DoWeaponTrace();
	void StopWeaponTrace();

	// 清空击中列表
	void ClearHitActors();

};
