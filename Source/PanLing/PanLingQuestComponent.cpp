// Fill out your copyright notice in the Description page of Project Settings.


#include "PanLingQuestComponent.h"
#include "PanLingCharacter.h" // 引入玩家，为了给玩家加经验
#include "AttributeComponent.h" // 引入属性组件，用于加经验
#include "PanLingSaveGame.h"

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
	// 1. 如果任务已经在进行中了，不允许重复接取
	if (ActiveQuestsProgress.Contains(QuestID)){return;}

	// 2. 如果任务曾经被完成过，我们需要检查它是否是“可重复任务”
	if (CompletedQuestsHistory.Contains(QuestID))
	{
		if (QuestDataTable)
		{
			// 去数据表查这个任务的配置
			FPanLingQuestData* QuestRow = QuestDataTable->FindRow<FPanLingQuestData>(QuestID, TEXT(""));

			// 如果找到了任务，且它配置为“不可重复 (bIsRepeatable == false)”
			if (QuestRow && !QuestRow->bIsRepeatable)
			{
				UE_LOG(LogTemp, Warning, TEXT("❌ 接取失败：%s 是一次性任务，且你已经完成过了！"), *QuestID.ToString());
				return; // 拒绝接取
			}
		}
	}

	// 3. 校验通过！接取任务并初始化进度为 0
	ActiveQuestsProgress.Add(QuestID, 0);
	UE_LOG(LogTemp, Warning, TEXT("⭐⭐⭐ 成功接取任务: %s ⭐⭐⭐"), *QuestID.ToString());
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
		// 将该任务永久记入完成历史档案
		CompletedQuestsHistory.AddUnique(CompletedID);
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

void UPanLingQuestComponent::SaveQuestData(UPanLingSaveGame* SaveObject)
{
	if (SaveObject)
	{
		// 将组件内的数据，直接赋值给存档对象里的变量
		SaveObject->SavedActiveQuests = ActiveQuestsProgress;
		SaveObject->SavedCompletedQuests = CompletedQuestsHistory;

		UE_LOG(LogTemp, Log, TEXT("任务数据已写入存档对象。"));
	}
}

void UPanLingQuestComponent::LoadQuestData(UPanLingSaveGame* SaveObject)
{
	if (SaveObject)
	{
		// 将存档对象里的变量，覆盖回组件内
		ActiveQuestsProgress = SaveObject->SavedActiveQuests;
		CompletedQuestsHistory = SaveObject->SavedCompletedQuests;

		UE_LOG(LogTemp, Log, TEXT("任务数据已从存档加载完成！"));
	}
}