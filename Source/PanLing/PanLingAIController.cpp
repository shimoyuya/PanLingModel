// Fill out your copyright notice in the Description page of Project Settings.


#include "PanLingAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

APanLingAIController::APanLingAIController()
{
	// 1. 创建感知组件
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));

	// 2. 创建视觉配置
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	// 配置视觉参数
	SightConfig->SightRadius = 1000.0f;       // 能看多远
	SightConfig->LoseSightRadius = 1200.0f;   // 跑多远会丢失目标
	SightConfig->PeripheralVisionAngleDegrees = 60.0f; // 视野角度 (120度)

	// 允许感知所有类型的目标（为了简单，这里全开）
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	// 将配置应用到感知组件
	AIPerceptionComp->ConfigureSense(*SightConfig);
	AIPerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
}

void APanLingAIController::BeginPlay()
{
	Super::BeginPlay();

	// 绑定感知更新事件
	if (AIPerceptionComp)
	{
		AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &APanLingAIController::OnTargetPerceived);
	}
}

void APanLingAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 如果我们在蓝图里配置了行为树，就开始运行它
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}
}

void APanLingAIController::OnTargetPerceived(AActor* Actor, FAIStimulus Stimulus)
{
	// 如果我们有黑板 (Blackboard)，并且看到了东西
	if (Blackboard)
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			// 将看到的目标写入黑板，键名为 "TargetActor"
			Blackboard->SetValueAsObject(FName("TargetActor"), Actor);
			UE_LOG(LogTemp, Warning, TEXT("AI: I see you, %s!"), *Actor->GetName());
		}
		else
		{
			// 丢失目标，清空黑板
			Blackboard->ClearValue(FName("TargetActor"));
			UE_LOG(LogTemp, Warning, TEXT("AI: Target lost."));
		}
	}
}
