// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h" // 包含感知数据类型
#include "PanLingAIController.generated.h"

class UBehaviorTree;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;

UCLASS()
class PANLING_API APanLingAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	APanLingAIController();

protected:
	virtual void BeginPlay() override;

	// 当 AI 控制器附身到敌人身上的瞬间调用
	virtual void OnPossess(APawn* InPawn) override;

	// 指定要运行的行为树
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTree;

	// 感知组件（眼睛和耳朵）
	UPROPERTY(VisibleAnywhere, Category = "AI")
	UAIPerceptionComponent* AIPerceptionComp;

	// 视觉配置
	UPROPERTY(VisibleAnywhere, Category = "AI")
	UAISenseConfig_Sight* SightConfig;

	// 当看到或丢失目标时触发的函数
	UFUNCTION()
	void OnTargetPerceived(AActor* Actor, FAIStimulus Stimulus);
};
