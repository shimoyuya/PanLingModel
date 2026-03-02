// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PanLingDamageNumberActor.generated.h"

class UWidgetComponent;

UCLASS()
class PANLING_API APanLingDamageNumberActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APanLingDamageNumberActor();

	// 标记当前这个飘字是否正在被使用（是否在屏幕上显示）
	bool bIsActive;

	// 激活并显示飘字（替代原本的 BeginPlay 逻辑）
	// @param Location 飘字出现的位置
	// @param DamageAmount 伤害数值
	UFUNCTION(BlueprintCallable, Category = "Damage Number")
	void ActivateDamageNumber(FVector Location, float DamageAmount);

	// 回收飘字，隐藏起来等待下次使用（替代原本的 Destroy）
	UFUNCTION(BlueprintCallable, Category = "Damage Number")
	void DeactivateDamageNumber();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 挂载 UI 的组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* DamageWidgetComp;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 对外暴露的接口：供武器调用，传入伤害值
	UFUNCTION(BlueprintCallable, Category = "Damage")
	void ShowDamage(float DamageAmount);
};
