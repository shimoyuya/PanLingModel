// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayInterface.h"
#include "Components/SceneComponent.h"
#include "SItemChest.generated.h"

UCLASS()
class PANLING_API ASItemChest : public AActor ,public IGameplayInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASItemChest();

	// 重写接口函数 Interact_Implementation
	// 注意：在 C++ 中实现 BlueprintNativeEvent 时，函数名后面要加 _Implementation
	virtual void Interact_Implementation(APawn* InstigatorPawn);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//箱体
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh;
	//铰链
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* LidSc;
	// 盖子
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* LidMesh; 
	// 标记盖子是否打开
	bool bLidOpened;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
