// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageNumberPoolComponent.h"
#include "PanLingDamageNumberActor.h"

// Sets default values for this component's properties
UDamageNumberPoolComponent::UDamageNumberPoolComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UDamageNumberPoolComponent::BeginPlay()
{
	Super::BeginPlay();

	// 如果在蓝图里配置了飘字类，我们就开始预生成
	if (DamageNumberClass)
	{
		for (int32 i = 0; i < InitialPoolSize; i++)
		{
			CreateNewDamageNumber();
		}
	}
	
}

APanLingDamageNumberActor* UDamageNumberPoolComponent::SpawnDamageNumber(FVector Location, float DamageAmount)
{
	// 遍历池子，找一个当前空闲的飘字
	for (TObjectPtr<APanLingDamageNumberActor> DamageActor : Pool)
	{
		if (DamageActor && !DamageActor->bIsActive)
		{
			// 找到了！激活它并返回
			DamageActor->ActivateDamageNumber(Location, DamageAmount);
			return DamageActor;
		}
	}

	// 如果代码走到这里，说明预生成的 10 个飘字都在用（可能玩家砍了一群怪）
	// 没关系，我们动态扩容（去买一辆新车）
	APanLingDamageNumberActor* ExpandedActor = CreateNewDamageNumber();
	if (ExpandedActor)
	{
		ExpandedActor->ActivateDamageNumber(Location, DamageAmount);
		return ExpandedActor;
	}

	return nullptr;
}


// Called every frame
void UDamageNumberPoolComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

APanLingDamageNumberActor* UDamageNumberPoolComponent::CreateNewDamageNumber()
{
	if (!DamageNumberClass || !GetWorld()) return nullptr;

	// 生成一个新的飘字 Actor
	APanLingDamageNumberActor* NewDamageNumber = GetWorld()->SpawnActor<APanLingDamageNumberActor>(DamageNumberClass, FVector::ZeroVector, FRotator::ZeroRotator);

	if (NewDamageNumber)
	{
		// 加入到我们的车库（数组）里
		Pool.Add(NewDamageNumber);
	}
	return NewDamageNumber;
}

