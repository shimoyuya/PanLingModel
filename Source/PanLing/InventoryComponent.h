// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h" // 必须包含此头文件以使用 FTableRowBase
#include "InventoryComponent.generated.h"

class UWeaponDataAsset;
class UTexture2D;

/**
 * 物品信息结构体，继承自 FTableRowBase 使其可用于 UE 的数据表 (DataTable)
 */
USTRUCT(BlueprintType)
struct FPanLingItemInfo : public FTableRowBase
{
	GENERATED_BODY()

	// 物品唯一 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemID;

	// 物品显示名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText ItemName;

	// 物品图标（用于 UI 显示）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UTexture2D* ItemIcon;

	// 物品描述
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText ItemDescription;

	// 如果是武器的话，关联的武器数据资产（如果为空，说明不是武器）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UWeaponDataAsset* WeaponData;
};

// 声明一个动态多播委托，当背包内容改变时通知 UI 刷新
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PANLING_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// 核心数据：存储当前背包内所有物品的数组
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FPanLingItemInfo> Items;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 添加物品到背包
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddItem(const FPanLingItemInfo& NewItem);

	// 移除指定索引的物品
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RemoveItemAtIndex(int32 Index);

	// 获取当前所有物品
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	const TArray<FPanLingItemInfo>& GetItems() const { return Items; }

	// 委托：供 UI 蓝图绑定，一旦触发 UI 就重新绘制格子
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;
};
