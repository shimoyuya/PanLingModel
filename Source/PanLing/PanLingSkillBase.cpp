// Fill out your copyright notice in the Description page of Project Settings.


#include "PanLingSkillBase.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "Engine/World.h"
// 引入你自己的属性组件头文件，用于扣除精力
#include "AttributeComponent.h"
#include "TimerManager.h"

UWorld* UPanLingSkillBase::GetWorld() const
{
	// 只要我们的 Outer (拥有者) 是 Actor 或 Component，就能拿到正确的 World
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return nullptr;
	}
	return GetOuter()->GetWorld();
}

bool UPanLingSkillBase::CanCast(ACharacter* Caster)
{
	if (!Caster || bIsOnCooldown) return false;

	// 检查精力是否足够 (假设 Caster 身上有你的 AttributeComp)
	UAttributeComponent* AttrComp = Caster->FindComponentByClass<UAttributeComponent>();
	if (AttrComp && AttrComp->GetStamina() < StaminaCost)
	{
		UE_LOG(LogTemp, Warning, TEXT("精力不足，无法释放技能: %s"), *SkillName.ToString());
		return false;
	}

	return true;
}

void UPanLingSkillBase::CastSkill(ACharacter* Caster)
{
	if (!CanCast(Caster)) return;

	// 1. 扣除精力
	UAttributeComponent* AttrComp = Caster->FindComponentByClass<UAttributeComponent>();
	if (AttrComp)
	{
		AttrComp->ApplyStaminaChange(-StaminaCost);
	}

	// 2. 播放动画
	if (CastMontage)
	{
		Caster->PlayAnimMontage(CastMontage);
	}

	// 3. 触发子类具体逻辑 (C++ 或 蓝图)
	OnSkillExecuted(Caster);

	// 4. 进入冷却
	bIsOnCooldown = true;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(CooldownTimerHandle, this, &UPanLingSkillBase::FinishCooldown, CooldownDuration, false);
	}
}

float UPanLingSkillBase::GetRemainingCooldown() const
{
	// 如果不在冷却中，或者获取不到世界，直接返回 0
	if (!bIsOnCooldown || !GetWorld())
	{
		return 0.0f;
	}

	// 从定时器管理器中获取该句柄的剩余时间
	return GetWorld()->GetTimerManager().GetTimerRemaining(CooldownTimerHandle);
}

float UPanLingSkillBase::GetCooldownRatio() const
{
	if (CooldownDuration <= 0.0f) return 0.0f;

	// 计算比例：剩余时间 / 总时间
	return GetRemainingCooldown() / CooldownDuration;
}

void UPanLingSkillBase::OnSkillExecuted_Implementation(ACharacter* Caster)
{
	// 默认空实现，具体特效/伤害逻辑留给蓝图子类去连线
	UE_LOG(LogTemp, Warning, TEXT("释放了技能: %s"), *SkillName.ToString());
}

void UPanLingSkillBase::FinishCooldown()
{
	bIsOnCooldown = false;
	UE_LOG(LogTemp, Warning, TEXT("技能冷却完毕: %s"), *SkillName.ToString());
}
