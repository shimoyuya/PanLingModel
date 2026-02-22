// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeComponent.h"

// Sets default values for this component's properties
UAttributeComponent::UAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true; 

	MaxHealth = 100.0f;
	Health = MaxHealth;

	bCanRegenStamina = true;
}


// Called when the game starts
void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	
	
}


// Called every frame
void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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

	float OldHealth = Health;

	// 计算新血量，并限制在 [0, MaxHealth] 之间
	float NewHealth = FMath::Clamp(Health + Delta, 0.0f, MaxHealth);

	// 计算实际变化量
	float ActualDelta = NewHealth - OldHealth;

	// 如果数值没有实际变化（比如满血治疗），就不广播
	if (ActualDelta == 0.0f)
	{
		return false;
	}

	Health = NewHealth;

	// 广播事件：告诉所有人（UI, Character）血量变了
	// Broadcast 的参数必须和头文件中 DECLARE 的参数一一对应
	OnHealthChanged.Broadcast(InstigatorActor, this, Health, ActualDelta);

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

