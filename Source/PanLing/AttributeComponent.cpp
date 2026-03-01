// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeComponent.h"
#include "PanLingEffectBase.h"

// Sets default values for this component's properties
UAttributeComponent::UAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true; 

	bCanRegenStamina = true;

	// 初始化一些默认基础值
	MaxHealthData.BaseValue = 100.f;
	AttackPowerData.BaseValue = 10.f;
	DefenseData.BaseValue = 5.f;
}


// Called when the game starts
void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	// 游戏开始时计算一次最终值
	MaxHealthData.Recalculate();
	AttackPowerData.Recalculate();
	DefenseData.Recalculate();

	CurrentHealth = MaxHealthData.CurrentValue;
	
}


// Called every frame
void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 逆序遍历正在生效的 Buff 数组 (逆序遍历是为了在移除元素时防止数组越界奔溃)
	for (int32 i = ActiveEffects.Num() - 1; i >= 0; --i)
	{
		if (ActiveEffects[i])
		{
			// 更新 Buff，如果返回 false 说明过期了
			if (!ActiveEffects[i]->UpdateEffect(DeltaTime))
			{
				ActiveEffects[i]->EndEffect();
				ActiveEffects.RemoveAt(i);
			}
		}
	}

	// 如果精力未满，且允许恢复，并且角色还活着
	if (bCanRegenStamina && Stamina < MaxStamina && IsAlive())
	{
		Stamina += StaminaRegenRate * DeltaTime;
		Stamina = FMath::Clamp(Stamina, 0.0f, MaxStamina);

		// 广播精力更新 (传 0 作为 Delta，因为是平滑更新)
		OnStaminaChanged.Broadcast(nullptr, this, Stamina, 0.0f);
	}
}

bool UAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	// 如果已经死亡且还在扣血，则忽略 (防止鞭尸逻辑)
	if (!IsAlive() && Delta < 0.0f)
	{
		return false;
	}

	float OldHealth = CurrentHealth;

	// 计算新血量，并限制在 [0, MaxHealth] 之间
	float NewHealth = FMath::Clamp(CurrentHealth + Delta, 0.0f, MaxHealthData.CurrentValue);

	// 计算实际变化量
	float ActualDelta = NewHealth - OldHealth;

	// 如果数值没有实际变化（比如满血治疗），就不广播
	if (ActualDelta == 0.0f)
	{
		return false;
	}

	CurrentHealth = NewHealth;

	// 广播事件：告诉所有人（UI, Character）血量变了
	// Broadcast 的参数必须和头文件中 DECLARE 的参数一一对应
	OnHealthChanged.Broadcast(InstigatorActor, this, CurrentHealth, ActualDelta);

	return true;
}

bool UAttributeComponent::ApplyStaminaChange(float Delta)
{
	// 只有在扣除精力时才判断是否足够 (Delta < 0)
	if (Delta < 0.0f && Stamina < FMath::Abs(Delta))
	{
		return false; // 精力不足，拒绝执行
	}

	Stamina += Delta;
	Stamina = FMath::Clamp(Stamina, 0.0f, MaxStamina);

	OnStaminaChanged.Broadcast(nullptr, this, Stamina, Delta);

	// 如果是消耗精力，暂停恢复 1.5 秒
	if (Delta < 0.0f)
	{
		bCanRegenStamina = false;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_StaminaRegenDelay, [this]()
			{
				bCanRegenStamina = true;
			}, 1.5f, false);
	}

	return true;
}

void UAttributeComponent::AddEXP(float EXPAmount)
{
	// 如果传入的经验值无效，或者已经满级，直接返回
	if (EXPAmount <= 0.0f || Level >= MaxLevel) return;

	CurrentEXP += EXPAmount;

	// 使用 while 循环处理可能连续升级的情况
	bool bLeveledUp = false;
	while (CurrentEXP >= MaxEXP && Level < MaxLevel)
	{
		CurrentEXP -= MaxEXP; // 扣除升级所需的经验
		Level++;              // 等级+1
		bLeveledUp = true;

		// 1. 提升属性上限 (你可以后续把它改成读取数据表 UDataTable，现在先用简单的公式)
		MaxHealthData.BaseValue += 20.0f;
		MaxHealthData.Recalculate();
		MaxStamina += 10.0f;

		// 2. 升级时恢复满血满状态
		CurrentHealth = MaxHealthData.CurrentValue;
		Stamina = MaxStamina;

		// 3. 增加下一级所需的经验值 (例如每级需求变成上一级的 1.2 倍)
		MaxEXP *= 1.2f;

		// 广播等级提升事件
		OnLevelChanged.Broadcast(Level, MaxLevel);
	}

	// 如果达到满级，把经验条锁定在最大值
	if (Level >= MaxLevel)
	{
		CurrentEXP = MaxEXP;
	}

	// 广播经验值变化事件 (UI 进度条需要)
	OnEXPChanged.Broadcast(CurrentEXP, MaxEXP);

	// 如果升级了，别忘了通知 UI 更新当前的满血满状态
	if (bLeveledUp)
	{
		// 假设你的 OnHealthChanged 签名是 (Actor, AttributeComp, NewHealth, Delta)
		OnHealthChanged.Broadcast(nullptr, this, CurrentHealth, 20.0f);
		OnStaminaChanged.Broadcast(nullptr, this, Stamina, MaxStamina);
	}
}

void UAttributeComponent::ApplyEffect(TSubclassOf<UPanLingEffectBase> EffectClass)
{
	if (!EffectClass) return;

	// 创建 Buff 的实例化对象，并将 This (属性组件) 设为它的 Outer
	UPanLingEffectBase* NewEffect = NewObject<UPanLingEffectBase>(this, EffectClass);

	if (NewEffect)
	{
		ActiveEffects.Add(NewEffect);
		NewEffect->StartEffect(this); // 启动 Buff 初始化逻辑
	}
}

void UAttributeComponent::RemoveEffect(UPanLingEffectBase* EffectToRemove)
{
	if (EffectToRemove && ActiveEffects.Contains(EffectToRemove))
	{
		EffectToRemove->EndEffect();
		ActiveEffects.RemoveSingle(EffectToRemove);
	}
}

void UAttributeComponent::AddModifierToAttribute(FName AttributeName, FPanLingModifier NewModifier)
{
    // 根据传入的名字找到对应的属性数据
    FPanLingAttributeData* TargetAttribute = nullptr;

    if (AttributeName == FName("MaxHealth")) TargetAttribute = &MaxHealthData;
    else if (AttributeName == FName("AttackPower")) TargetAttribute = &AttackPowerData;
    else if (AttributeName == FName("Defense")) TargetAttribute = &DefenseData;

    if (TargetAttribute)
    {
		// 记录修改前的最大生命值
		float OldMaxValue = TargetAttribute->CurrentValue;

        // 添加修改器并重新计算
        TargetAttribute->Modifiers.Add(NewModifier);
        TargetAttribute->Recalculate();

        // [额外逻辑] 如果修改的是最大生命值，需要确保当前生命值不超过上限等...
		if (AttributeName == FName("MaxHealth"))
		{
			// 例如：最大生命值增加了，我们把增加的差值也加到当前血量上（类似吃撑血药的效果）
			float MaxHealthDiff = TargetAttribute->CurrentValue - OldMaxValue;
			if (MaxHealthDiff > 0)
			{
				CurrentHealth += MaxHealthDiff;
			}
			// 确保当前血量不超过新的上限
			CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, TargetAttribute->CurrentValue);

			// 通知 UI 刷新血条
			OnHealthChanged.Broadcast(nullptr, this, CurrentHealth, MaxHealthDiff);
		}
    }
}

void UAttributeComponent::RemoveModifierFromAttribute(FName AttributeName, FName SourceID)
{
    FPanLingAttributeData* TargetAttribute = nullptr;

    if (AttributeName == FName("MaxHealth")) TargetAttribute = &MaxHealthData;
    else if (AttributeName == FName("AttackPower")) TargetAttribute = &AttackPowerData;
    else if (AttributeName == FName("Defense")) TargetAttribute = &DefenseData;

    if (TargetAttribute)
    {
        // 移除所有 SourceID 匹配的修改器
        TargetAttribute->Modifiers.RemoveAll([SourceID](const FPanLingModifier& Mod) {
            return Mod.SourceID == SourceID;
        });

        // 移除后重新计算
        TargetAttribute->Recalculate();

		// 卸下装备时，如果当前血量超过了上限，需要裁剪
		if (AttributeName == FName("MaxHealth"))
		{
			if (CurrentHealth > TargetAttribute->CurrentValue)
			{
				CurrentHealth = TargetAttribute->CurrentValue;
				// 通知 UI 刷新血条
				OnHealthChanged.Broadcast(nullptr, this, CurrentHealth, 0.f);
			}
		}
    }
}