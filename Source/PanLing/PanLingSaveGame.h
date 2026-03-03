// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PanLingSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class PANLING_API UPanLingSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	// 默认插槽名称和玩家索引
	UPROPERTY(VisibleAnywhere, Category = "Basic")
	FString SaveSlotName = TEXT("PlayerSlot_01");

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	uint32 UserIndex = 0;

	// --- 下面是我们要保存的玩家数据 ---

	// 1. 玩家在世界中的位置和朝向
	UPROPERTY(VisibleAnywhere, Category = "SaveData|Transform")
	FTransform PlayerTransform;

	// 2. 玩家的养成数据 (等级、经验)
	UPROPERTY(VisibleAnywhere, Category = "SaveData|Attributes")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, Category = "SaveData|Attributes")
	float CurrentEXP = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "SaveData|Attributes")
	float MaxEXP = 100.f;

	// 3. 装备状态
	UPROPERTY(VisibleAnywhere, Category = "SaveData|Inventory")
	FName EquippedWeaponID;

	// 4. (进阶) 背包中的物品列表
	 UPROPERTY(VisibleAnywhere, Category = "SaveData|Inventory")
	 TArray<FName> InventoryItemIDs;

	 // --- 任务系统存档数据 ---
	// 保存当前正在进行中的任务及击杀进度
	 UPROPERTY(VisibleAnywhere, Category = "SaveData|Quest")
	 TMap<FName, int32> SavedActiveQuests;

	 // 保存已经完成的任务历史（主线防重复）
	 UPROPERTY(VisibleAnywhere, Category = "SaveData|Quest")
	 TArray<FName> SavedCompletedQuests;
};
