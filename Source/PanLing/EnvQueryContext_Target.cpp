// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryContext_Target.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "AIController.h"
#include "EnvironmentQuery/EnvQueryTypes.h"

void UEnvQueryContext_Target::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	// 1. 获取是谁在发起这个 EQS 查询 (通常是 AI 控制器或者 AI Pawn)
	AActor* QueryOwner = Cast<AActor>(QueryInstance.Owner.Get());
	if (!QueryOwner) return;

	// 2. 尝试获取 AI 控制器
	AAIController* AICon = nullptr;
	if (APawn* OwnerPawn = Cast<APawn>(QueryOwner))
	{
		// 如果 Owner 是 Pawn，获取它身上的 Controller
		AICon = Cast<AAIController>(OwnerPawn->GetController());
	}
	else
	{
		// 否则 Owner 本身可能就是 Controller
		AICon = Cast<AAIController>(QueryOwner);
	}

	// 3. 如果成功获取到了 AIController 以及它的黑板组件
	if (AICon && AICon->GetBlackboardComponent())
	{
		// 读取我们之前在感知系统中存入的 "TargetActor"
		UObject* TargetObject = AICon->GetBlackboardComponent()->GetValueAsObject(FName("TargetActor"));
		AActor* TargetActor = Cast<AActor>(TargetObject);

		if (TargetActor)
		{
			// 4. 将获取到的玩家 Actor 包装进 EQS 的 ContextData 中！
			// 这行代码是 EQS 引擎底层的固定写法，告诉 EQS "目标就是这个 Actor"
			UEnvQueryItemType_Actor::SetContextHelper(ContextData, TargetActor);
		}
	}
}
