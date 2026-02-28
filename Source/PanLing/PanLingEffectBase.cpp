// Fill out your copyright notice in the Description page of Project Settings.


#include "PanLingEffectBase.h"
#include "AttributeComponent.h"

UPanLingEffectBase::UPanLingEffectBase()
{
	Duration = 5.0f;     // 默认持续 5 秒
	TickInterval = 0.0f; // 默认不进行周期触发
	TimeRemaining = 0.0f;
	TimeSinceLastTick = 0.0f;
	TargetAttributeComp = nullptr;
}

UWorld* UPanLingEffectBase::GetWorld() const
{
	// 使用 Outer（外部持有者，即 AttributeComponent）的 World
	if (HasAnyFlags(RF_ClassDefaultObject)) return nullptr;
	return GetOuter() ? GetOuter()->GetWorld() : nullptr;
}

void UPanLingEffectBase::StartEffect(UAttributeComponent* Target)
{
	TargetAttributeComp = Target;
	TimeRemaining = Duration;
	TimeSinceLastTick = 0.0f;

	// 调用蓝图可重写的应用逻辑
	OnApply();
}

bool UPanLingEffectBase::UpdateEffect(float DeltaTime)
{
	// 如果 Duration <= 0，认为是永久 Buff，直接返回 true 保持存活，并处理 Tick
	if (Duration > 0.0f)
	{
		TimeRemaining -= DeltaTime;
		if (TimeRemaining <= 0.0f)
		{
			return false; // 时间到了，通知外部删除此 Buff
		}
	}

	// 处理周期性触发 (DoT / HoT)
	if (TickInterval > 0.0f)
	{
		TimeSinceLastTick += DeltaTime;
		if (TimeSinceLastTick >= TickInterval)
		{
			OnTick();
			// 减去 TickInterval，而不是清零，这样能保证长期运行下的时间精准度
			TimeSinceLastTick -= TickInterval;
		}
	}

	return true; // Buff 依然有效
}

void UPanLingEffectBase::EndEffect()
{
	// 调用蓝图可重写的移除逻辑
	OnRemove();
	TargetAttributeComp = nullptr;
}

// 蓝图 Native Event 的默认 C++ 空实现
void UPanLingEffectBase::OnApply_Implementation() {}
void UPanLingEffectBase::OnTick_Implementation() {}
void UPanLingEffectBase::OnRemove_Implementation() {}
