// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_AttackDash.h"
#include "CombatComponent.h"

void UAnimNotify_AttackDash::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		UCombatComponent* CombatComp = MeshComp->GetOwner()->FindComponentByClass<UCombatComponent>();
		if (CombatComp)
		{
			// 通知战斗组件执行冲刺
			CombatComp->AttackDash(DashForce);
		}
	}
}
