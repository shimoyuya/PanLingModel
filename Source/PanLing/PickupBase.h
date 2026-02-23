// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayInterface.h" // 继承你的交互接口
#include "InventoryComponent.h" // 引入结构体定义
#include "PickupBase.generated.h"

class UStaticMeshComponent;
class UDataTable;

UCLASS()
class PANLING_API APickupBase : public AActor , public IGameplayInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupBase();

	// 重写交互接口
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 物品的模型
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	// 核心配置：指向我们在引擎里创建的数据表
	UPROPERTY(EditAnywhere, Category = "Item Data")
	UDataTable* ItemDataTable;

	// 核心配置：该物品在数据表中的行名称 (Row Name)
	UPROPERTY(EditAnywhere, Category = "Item Data")
	FName ItemRowName;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



};
