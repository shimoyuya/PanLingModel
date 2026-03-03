// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PanLingQuestListWidget.generated.h"

class UVerticalBox;
class UPanLingQuestComponent;
class UPanLingQuestEntryWidget;

/**
	任务面板 UI
 */
UCLASS()
class PANLING_API UPanLingQuestListWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 刷新整个列表
	UFUNCTION(BlueprintCallable, Category = "UI")
	void RefreshList(UPanLingQuestComponent* QuestComp);

protected:
	// 绑定蓝图里的垂直框，用来容纳所有的子任务
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* QuestListContainer;

	// 配置：单条任务的 UI 类
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPanLingQuestEntryWidget> EntryWidgetClass;
};
