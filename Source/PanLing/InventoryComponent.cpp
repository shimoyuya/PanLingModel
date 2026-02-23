// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;// 背包组件不需要每帧 Tick

	
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	
	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

void UInventoryComponent::AddItem(const FPanLingItemInfo& NewItem)
{
	Items.Add(NewItem);

	// 广播委托，通知 UI 背包更新了
	OnInventoryUpdated.Broadcast();

	// 打印调试信息，方便我们测试
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("Picked up: %s"), *NewItem.ItemName.ToString()));
	}
}

void UInventoryComponent::RemoveItemAtIndex(int32 Index)
{
	if (Items.IsValidIndex(Index))
	{
		Items.RemoveAt(Index);
		OnInventoryUpdated.Broadcast();
	}
}

