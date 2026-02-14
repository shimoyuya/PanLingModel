// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeComponent.h"

// Sets default values for this component's properties
UAttributeComponent::UAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false; // 属性组件不需要每帧 Tick，省性能

	MaxHealth = 100.0f;
	Health = MaxHealth;
}


// Called when the game starts
void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
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

bool UAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

float UAttributeComponent::GetMaxHealth() const
{
	return MaxHealth;
}

float UAttributeComponent::GetHealth() const
{
	return Health;
}

