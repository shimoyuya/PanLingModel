// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PanLingQuestTypes.h"
#include "PanLingQuestComponent.generated.h"

class UDataTable;

// 声明一个带有两个参数（任务ID，奖励经验）的多播委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestCompletedSignature, FName, QuestID, float, RewardEXP);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PANLING_API UPanLingQuestComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPanLingQuestComponent();

	// 核心函数：用于接收并添加任务。必须标记为 UFUNCTION 才能绑定到动态委托上！
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void AddQuest(FName QuestID);

	// 新增：当任何怪物被击杀时，供外部调用的接口
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void OnEnemyKilled(FName EnemyKilledID);

	// 任务完成时触发的事件
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnQuestCompletedSignature OnQuestCompleted;

	// 获取当前所有活跃任务的进度信息
	UFUNCTION(BlueprintCallable, Category = "Quest")
	TArray<FPanLingQuestDisplayInfo> GetActiveQuestsInfo();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// 配置：任务数据表 (要在主角蓝图里赋值)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	UDataTable* QuestDataTable;

	// 核心状态：当前进行的任务 以及 它们当前的进度 (QuestID -> CurrentKills)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	TMap<FName, int32> ActiveQuestsProgress;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
