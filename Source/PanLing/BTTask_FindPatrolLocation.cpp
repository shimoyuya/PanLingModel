// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindPatrolLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "AIController.h"

UBTTask_FindPatrolLocation::UBTTask_FindPatrolLocation()
{
	NodeName = TEXT("Find Random Patrol Location");
}

EBTNodeResult::Type UBTTask_FindPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 1. 获取 AI 控制器和 AI 的 Pawn (身体)
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn) return EBTNodeResult::Failed;

	// 2. 获取当前世界的导航系统
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys) return EBTNodeResult::Failed;

	// 3. 在 AI 的当前位置附近找一个可达的随机点
	FVector Origin = AIPawn->GetActorLocation();
	FNavLocation RandomLocation;

	if (NavSys->GetRandomReachablePointInRadius(Origin, SearchRadius, RandomLocation))
	{
		// 4. 如果找到了，就把这个点的坐标写入到黑板里
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(PatrolLocationKey.SelectedKeyName, RandomLocation.Location);

		// 任务成功！
		return EBTNodeResult::Succeeded;
	}

	// 没找到路
	return EBTNodeResult::Failed;
}
