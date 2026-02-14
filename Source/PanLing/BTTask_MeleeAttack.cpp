// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MeleeAttack.h"
#include "AIController.h"
#include "PanLingEnemy.h"

UBTTask_MeleeAttack::UBTTask_MeleeAttack()
{
	NodeName = "Melee Attack"; // 在行为树中显示的节点名字
}

EBTNodeResult::Type UBTTask_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 1. 获取控制这个行为树的 AIController
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (ensure(MyController))
	{
		// 2. 获取 AI 控制的躯体（敌人角色）
		APanLingEnemy* MyEnemy = Cast<APanLingEnemy>(MyController->GetPawn());
		if (MyEnemy)
		{
			// 3. 执行攻击
			MyEnemy->Attack();

			// 任务成功完成
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}

