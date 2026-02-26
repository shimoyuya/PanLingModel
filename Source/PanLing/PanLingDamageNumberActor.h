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
