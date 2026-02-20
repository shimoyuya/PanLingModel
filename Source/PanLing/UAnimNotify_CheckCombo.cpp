// Fill out your copyright notice in the Description page of Project Settings.


#include "UAnimNotify_CheckCombo.h"
#include "GameFramework/Actor.h"
#include "CombatComponent.h" // 替换为你的实际连招攻击头文件

void UUAnimNotify_CheckCombo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		// 查找角色身上的战斗组件并调用检查函数
		UCombatComponent* CombatComp = MeshComp->GetOwner()->FindComponentByClass<UCombatComponent>();
		if (CombatComp)
		{
			CombatComp->CheckCombo();
		}
	}
}
