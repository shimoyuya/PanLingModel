// Fill out your copyright notice in the Description page of Project Settings.


#include "SItemChest.h"

// Sets default values
ASItemChest::ASItemChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;

	LidSc = CreateDefaultSubobject<USceneComponent>(TEXT("LidSc"));
	LidSc->SetupAttachment(BaseMesh);

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidMesh"));
	LidMesh->SetupAttachment(LidSc);
}

void ASItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	// 简单的逻辑：切换开关状态，旋转盖子
	bLidOpened = !bLidOpened;

	// 根据状态设置旋转角度 (Pitch)
	float TargetPitch = bLidOpened ? 110.0f : 0.0f;
	LidMesh->SetRelativeRotation(FRotator(TargetPitch, 0, 0));

	// 可选：打印日志确认
	UE_LOG(LogTemp, Warning, TEXT("Chest Interacted! Open State: %s"), bLidOpened ? TEXT("True") : TEXT("False"));
}

// Called when the game starts or when spawned
void ASItemChest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASItemChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

