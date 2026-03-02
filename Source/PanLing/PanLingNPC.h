// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayInterface.h"
#include "PanLingNPC.generated.h"

class USkeletalMeshComponent;
class UDataTable;
class UPanLingDialogueWidget;

UCLASS()
class PANLING_API APanLingNPC : public AActor , public IGameplayInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APanLingNPC();

	// 重写交互接口
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// NPC 的模型
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* MeshComp;

	// 配置：这个NPC对应的数据表
	UPROPERTY(EditAnywhere, Category = "NPC|Dialogue")
	UDataTable* DialogueDataTable;

	// 配置：需要生成的UI类 (可以在蓝图里配置为你稍后创建的 WBP_Dialogue)
	UPROPERTY(EditAnywhere, Category = "NPC|Dialogue")
	TSubclassOf<UPanLingDialogueWidget> DialogueWidgetClass;

	// 配置：第一次跟这个NPC说话时，从数据表的哪一行开始？
	UPROPERTY(EditAnywhere, Category = "NPC|Dialogue")
	FName StartDialogueRow;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
