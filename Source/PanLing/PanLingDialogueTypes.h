// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PanLingDialogueTypes.generated.h"

/**
 * 继承自 FTableRowBase，这样它就能在引擎里作为数据表(DataTable)的行数据被创建
 */
USTRUCT(BlueprintType)
struct FPanLingDialogueData : public FTableRowBase
{
	GENERATED_BODY()

	// 说话者的名字 (例如: "村长")
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FName SpeakerName;

	// 对话的具体文本 (使用 FText 以便未来支持多语言本地化)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue", meta = (MultiLine = "true"))
	FText DialogueText;

	// 下一句对话的 RowName (如果是最后一句，填 None 即可)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FName NextRowID;

	// --- 任务系统预留接口 ---
	// 读到这句话时，是否会触发/接取任务？
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	bool bHasQuest = false;

	// 如果触发任务，任务的ID是什么？
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest", meta = (EditCondition = "bHasQuest"))
	FName QuestID;
};
