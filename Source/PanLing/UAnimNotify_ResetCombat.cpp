// Fill out your copyright notice in the Description page of Project Settings.


#include "UAnimNotify_ResetCombat.h"
#include "GameFramework/Actor.h"
#include "CombatComponent.h"

void UUAnimNotify_ResetCombat::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		// 查找角色身上的战斗组件并调用检查函数
		UCombatComponent* CombatComp = MeshComp->GetOwner()->FindComponentByClass<UCombatComponent>();
		if (CombatComp)
		{
			CombatComp->ResetCombatState();
		}
	}
}
