// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindPatrolLocation.generated.h"

/**
 * 
 */
UCLASS()
class PANLING_API UBTTask_FindPatrolLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_FindPatrolLocation();

	// 重写任务执行函数
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	// 巡逻的寻找半径 (允许在蓝图里配置)
	UPROPERTY(EditAnywhere, Category = "AI")
	float SearchRadius = 1500.0f;

	// 指定我们要把坐标存到哪个黑板键里
	UPROPERTY(EditAnywhere, Category = "AI")
	struct FBlackboardKeySelector PatrolLocationKey;
};
