// Fill out your copyright notice in the Description page of Project Settings.


#include "PanLingQuestEntryWidget.h"
#include "Components/TextBlock.h"

void UPanLingQuestEntryWidget::SetupEntry(const FPanLingQuestDisplayInfo& Info)
{
	if (QuestNameText)
	{
		// 显示任务ID（你也可以以后在数据表里加个友好的中文名来替换）
		QuestNameText->SetText(FText::FromName(Info.QuestID));
	}

	if (ProgressText)
	{
		// 格式化输出进度：例如 "(1/3)"
		FString ProgressString = FString::Printf(TEXT("(%d / %d)"), Info.CurrentKills, Info.RequiredKills);
		ProgressText->SetText(FText::FromString(ProgressString));
	}
}
