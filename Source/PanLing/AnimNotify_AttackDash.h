// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_AttackDash.generated.h"

/**
 * 
 */
UCLASS()
class PANLING_API UAnimNotify_AttackDash : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	// 暴露给蓝图，可以在蒙太奇中动态调整这一招的冲刺力度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DashForce = 600.0f;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

};
