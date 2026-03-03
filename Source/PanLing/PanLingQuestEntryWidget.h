// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PanLingQuestTypes.h"
#include "PanLingQuestEntryWidget.generated.h"

class UTextBlock;

/**
	单条任务 (Entry)
 */
UCLASS()
class PANLING_API UPanLingQuestEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 初始化单条记录
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetupEntry(const FPanLingQuestDisplayInfo& Info);

protected:
	// 绑定蓝图里的任务名文本
	UPROPERTY(meta = (BindWidget))
	UTextBlock* QuestNameText;

	// 绑定蓝图里的进度文本
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ProgressText;
};
