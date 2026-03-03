// Fill out your copyright notice in the Description page of Project Settings.


#include "PanLingQuestComponent.h"
#include "PanLingCharacter.h" // 引入玩家，为了给玩家加经验
#include "AttributeComponent.h" // 引入属性组件，用于加经验

// Sets default values for this component's properties
UPanLingQuestComponent::UPanLingQuestComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UPanLingQuestComponent::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void UPanLingQuestComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UPanLingQuestComponent::AddQuest(FName QuestID)
{
	// 检查是否已经接取过这个任务
	if (!ActiveQuestsProgress.Contains(QuestID))
	{
		ActiveQuestsProgress.Add(QuestID , 0);

		// 打印日志：用于我们在编辑器里确认接取成功
		UE_LOG(LogTemp, Warning, TEXT("⭐⭐⭐ 成功接取任务: %s ⭐⭐⭐"), *QuestID.ToString());

		// TODO: 未来可以在这里调用屏幕上的 "任务接取成功" 飘字 UI
	}
}

void UPanLingQuestComponent::OnEnemyKilled(FName EnemyKilledID)
{
	// 如果没有配置数据表，直接返回以防崩溃
	if (!QuestDataTable) return;

	TArray<FName> CompletedQuests;

	// 遍历玩家当前正在做的所有任务
	for (auto& Pair : ActiveQuestsProgress)
	{
		FName QuestID = Pair.Key;
		int32& CurrentKills = Pair.Value;

		// 去数据表里查一下这个任务的要求是什么
		static const FString ContextString(TEXT("Quest Context"));
		FPanLingQuestData* QuestRow = QuestDataTable->FindRow<FPanLingQuestData>(QuestID, ContextString);

		// 如果任务要求杀的怪，刚好等于这次死的怪的ID
		if (QuestRow && QuestRow->TargetEnemyID == EnemyKilledID)
		{
			CurrentKills++; // 进度 +1
			UE_LOG(LogTemp, Warning, TEXT("任务进度更新: %s (%d/%d)"), *QuestID.ToString(), CurrentKills, QuestRow->RequiredKills);

			// 检查是否达标
			if (CurrentKills >= QuestRow->RequiredKills)
			{
				CompletedQuests.Add(QuestID); // 记录下来准备结算
			}
		}
	}

	// 结算刚刚完成的任务
	for (FName CompletedID : CompletedQuests)
	{
		FPanLingQuestData* QuestRow = QuestDataTable->FindRow<FPanLingQuestData>(CompletedID, TEXT(""));
		if (QuestRow)
		{
			UE_LOG(LogTemp, Warning, TEXT("🎉🎉🎉 任务完成: %s ! 获得经验: %f 🎉🎉🎉"), *CompletedID.ToString(), QuestRow->RewardEXP);

			// 给玩家发放经验奖励 (假设你的玩家身上有 AttributeComponent)
			if (APawn* MyOwner = Cast<APawn>(GetOwner()))
			{
				if (UAttributeComponent* AttrComp = MyOwner->FindComponentByClass<UAttributeComponent>())
				{
					AttrComp->AddEXP(QuestRow->RewardEXP);
				}
			}
			OnQuestCompleted.Broadcast(CompletedID, QuestRow->RewardEXP);
		}
		// 从进行中的任务列表里移除
		ActiveQuestsProgress.Remove(CompletedID);
	}
}

TArray<FPanLingQuestDisplayInfo> UPanLingQuestComponent::GetActiveQuestsInfo()
{
	TArray<FPanLingQuestDisplayInfo> Result;

	if (!QuestDataTable) return Result;

	// 遍历当前进行中的任务字典
	for (auto& Pair : ActiveQuestsProgress)
	{
		FPanLingQuestDisplayInfo Info;
		Info.QuestID = Pair.Key;
		Info.CurrentKills = Pair.Value;

		// 去数据表查一下这个任务的要求总数是多少
		FPanLingQuestData* QuestRow = QuestDataTable->FindRow<FPanLingQuestData>(Pair.Key, TEXT(""));
		if (QuestRow)
		{
			Info.RequiredKills = QuestRow->RequiredKills;
		}

		Result.Add(Info);
	}

	return Result;
}