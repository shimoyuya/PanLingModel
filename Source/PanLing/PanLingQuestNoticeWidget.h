// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PanLingQuestNoticeWidget.generated.h"

class UTextBlock;

/**
	任务完成弹出UI
 */
UCLASS()
class PANLING_API UPanLingQuestNoticeWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 对外接口：初始化文本并触发动画
	UFUNCTION(BlueprintCallable, Category = "UI")
	void PlayQuestCompletedAnim(const FString& QuestName, float RewardEXP);

protected:
	// 绑定蓝图中的任务名称文本
	UPROPERTY(meta = (BindWidget))
	UTextBlock* QuestNameText;

	// 绑定蓝图中的奖励文本
	UPROPERTY(meta = (BindWidget))
	UTextBlock* RewardText;

	// 定义一个事件，供蓝图去实现具体的动画播放逻辑！
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void PlayAnim();
};
