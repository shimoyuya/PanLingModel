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

TArray<FName> UInventoryComponent::GetInventoryItemIDs() const
{
	TArray<FName> SavedIDs;
	// 遍历当前背包，把所有物品的 ItemID 提取出来
	for (const FPanLingItemInfo& Item : Items)
	{
		SavedIDs.Add(Item.ItemID);
	}
	return SavedIDs;
}

void UInventoryComponent::LoadInventoryFromIDs(const TArray<FName>& SavedIDs)
{
	// 1. 读档前先清空当前背包
	Items.Empty();

	// 2. 确保我们在蓝图中配置了数据表
	if (ItemDataTable)
	{
		for (const FName& ID : SavedIDs)
		{
			// 根据 ID 去数据表里寻找对应的行数据
			// "LoadInventoryContext" 只是一个用于报错时定位的上下文文本
			FPanLingItemInfo* RowInfo = ItemDataTable->FindRow<FPanLingItemInfo>(ID, TEXT("LoadInventoryContext"));

			if (RowInfo)
			{
				// 如果找到了，就把完整的结构体重新加入背包
				Items.Add(*RowInfo);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("读档警告：数据表里找不到物品 ID: %s"), *ID.ToString());
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryComponent 读档失败：ItemDataTable 未配置！"));
	}

	// 3. 通知 UI 重新绘制背包格子
	OnInventoryUpdated.Broadcast();
}