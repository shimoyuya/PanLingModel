// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"
#include "PanLingWeapon.generated.h"

class UBoxComponent;
class UWeaponDataAsset;
class APanLingDamageNumberActor;

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

	// 记录一次攻击中已经击中过的 Actor，防止多帧重复扣血
	UPROPERTY()
	TArray<AActor*> HitActors;

	// 标记当前是否正在挥刀
	bool bIsTracing;

	// 用于顿帧恢复的定时器句柄
	FTimerHandle TimerHandle_HitStop;

public:	
	// Sets default values for this actor's properties
	APanLingWeapon();

	// 暴露给外部初始化武器的方法
	void InitializeWeapon(UWeaponDataAsset* InWeaponData);

	// 获取这把武器的数据 (方便组件调用)
	UWeaponDataAsset* GetWeaponData() const { return WeaponData; }

	// 恢复时间流速的函数
	void ResetHitStop();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 数据资产
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Data")
	UWeaponDataAsset* WeaponData;

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
