// Fill out your copyright notice in the Description page of Project Settings.


#include "PanLingNPC.h"
#include "Components/SkeletalMeshComponent.h"
#include "PanLingDialogueWidget.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APanLingNPC::APanLingNPC()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
}

// Called when the game starts or when spawned
void APanLingNPC::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APanLingNPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APanLingNPC::Interact_Implementation(APawn* InstigatorPawn)
{
	// 防御性编程：检查是否配置了 UI 类和数据表
	if (!DialogueWidgetClass || !DialogueDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("NPC %s is missing DialogueWidgetClass or DialogueDataTable!"), *GetName());
		return;
	}

	APlayerController* PC = Cast<APlayerController>(InstigatorPawn->GetController());
	if (PC)
	{
		// 创建 对话 UI 并添加到视口
		UPanLingDialogueWidget* DialogueWidget = CreateWidget<UPanLingDialogueWidget>(PC, DialogueWidgetClass);
		if (DialogueWidget)
		{
			DialogueWidget->AddToViewport();

			// 调用我们刚才在 UI 里写的公开接口，把数据表和起始行传给它
			DialogueWidget->StartDialogue(DialogueDataTable, StartDialogueRow, PC);
		}
	}
}