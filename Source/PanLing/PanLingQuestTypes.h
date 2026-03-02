// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PanLingQuestTypes.generated.h"

/**
 * 任务数据结构：用于在引擎中配置每个任务的需求和奖励
 */
USTRUCT(BlueprintType)
struct FPanLingQuestData : public FTableRowBase
{
	GENERATED_BODY()

	// 任务要求击杀的怪物ID (例如 "Slime")
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Requirements")
	FName TargetEnemyID;

	// 需要击杀的数量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Requirements")
	int32 RequiredKills = 0;

	// 任务奖励的经验值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Rewards")
	float RewardEXP = 0.f;

	// 这里以后还可以扩展奖励的物品ID: FName RewardItemID...
};