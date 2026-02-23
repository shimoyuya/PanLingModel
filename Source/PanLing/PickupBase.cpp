// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupBase.h"
#include "Components/StaticMeshComponent.h"
#include "PanLingCharacter.h"
#include "InventoryComponent.h"

// Sets default values
APickupBase::APickupBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	// 开启物理模拟以便物品能掉落在地上（如果需要的话可以保持开启）
	MeshComp->SetSimulatePhysics(true);
	// 设置碰撞类型，确保玩家可以与之交互
	MeshComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));

}

void APickupBase::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!InstigatorPawn || !ItemDataTable || ItemRowName.IsNone()) return;

	// 1. 获取玩家身上的背包组件
	APanLingCharacter* PlayerCharacter = Cast<APanLingCharacter>(InstigatorPawn);
	if (PlayerCharacter)
	{
		UInventoryComponent* Inventory = PlayerCharacter->GetInventoryComponent();
		if (Inventory)
		{
			// 2. 从数据表中读取数据！
			static const FString ContextString(TEXT("PickupBase Item Lookup"));
			FPanLingItemInfo* FoundItem = ItemDataTable->FindRow<FPanLingItemInfo>(ItemRowName, ContextString);

			if (FoundItem)
			{
				// 3. 将物品加入背包
				Inventory->AddItem(*FoundItem);

				// 4. 拾取成功，销毁地上的模型
				Destroy();
			}
		}
	}
}

// Called when the game starts or when spawned
void APickupBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickupBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

