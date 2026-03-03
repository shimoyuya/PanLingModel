// Fill out your copyright notice in the Description page of Project Settings.


#include "PanLingQuestNoticeWidget.h"
#include "Components/TextBlock.h"

void UPanLingQuestNoticeWidget::PlayQuestCompletedAnim(const FString& QuestName, float RewardEXP)
{
	// 设置任务标题
	if (QuestNameText)
	{
		QuestNameText->SetText(FText::FromString(QuestName + TEXT(" 已完成！")));
	}

	// 设置奖励文本，拼出 "+500 EXP" 的效果
	if (RewardText)
	{
		FString RewardStr = FString::Printf(TEXT("获得奖励: +%.0f EXP"), RewardEXP);
		RewardText->SetText(FText::FromString(RewardStr));
	}

	// 呼叫蓝图播放动画
	PlayAnim();
}