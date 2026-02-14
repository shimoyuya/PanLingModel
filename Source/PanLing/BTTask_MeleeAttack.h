// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class PANLING_API UBTTask_MeleeAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_MeleeAttack();

protected:
	// 重写任务执行函数
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
