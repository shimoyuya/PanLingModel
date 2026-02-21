// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_WeaponTrace.h"
#include "CombatComponent.h"
#include "PanLingWeapon.h"

void UANS_WeaponTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		// 寻找战斗组件
		if (UCombatComponent* CombatComp = MeshComp->GetOwner()->FindComponentByClass<UCombatComponent>())
		{
			// 寻找并开启武器追踪
			if (APanLingWeapon* Weapon = CombatComp->GetEquippedWeapon())
			{
				Weapon->StartWeaponTrace();
			}
		}
	}
}

void UANS_WeaponTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		if (UCombatComponent* CombatComp = MeshComp->GetOwner()->FindComponentByClass<UCombatComponent>())
		{
			if (APanLingWeapon* Weapon = CombatComp->GetEquippedWeapon())
			{
				// 每帧进行一次检测！
				Weapon->DoWeaponTrace();
			}
		}
	}
}

void UANS_WeaponTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		if (UCombatComponent* CombatComp = MeshComp->GetOwner()->FindComponentByClass<UCombatComponent>())
		{
			if (APanLingWeapon* Weapon = CombatComp->GetEquippedWeapon())
			{
				// 关闭武器追踪
				Weapon->StopWeaponTrace();
			}
		}
	}
}
