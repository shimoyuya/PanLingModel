// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageNumberPoolComponent.generated.h"

class APanLingDamageNumberActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PANLING_API UDamageNumberPoolComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDamageNumberPoolComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// 核心功能：向池子申请一个飘字
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	APanLingDamageNumberActor* SpawnDamageNumber(FVector Location, float DamageAmount);

protected:
	// 在蓝图中配置的飘字类（比如你做好的 BP_DamageNumberActor）
	UPROPERTY(EditDefaultsOnly, Category = "Object Pool")
	TSubclassOf<APanLingDamageNumberActor> DamageNumberClass;

	// 游戏开始时预生成的数量
	UPROPERTY(EditDefaultsOnly, Category = "Object Pool")
	int32 InitialPoolSize = 10;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// 存放所有飘字的数组（在 UE5 中，推荐使用 TObjectPtr 代替裸指针，更安全）
	UPROPERTY()
	TArray<TObjectPtr<APanLingDamageNumberActor>> Pool;

	// 内部封装的一个生成函数
	APanLingDamageNumberActor* CreateNewDamageNumber();
		
};
