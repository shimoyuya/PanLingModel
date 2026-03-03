// Fill out your copyright notice in the Description page of Project Settings.


#include "PanLingQuestListWidget.h"
#include "Components/VerticalBox.h"
#include "PanLingQuestEntryWidget.h"
#include "PanLingQuestComponent.h"

void UPanLingQuestListWidget::RefreshList(UPanLingQuestComponent* QuestComp)
{
	if (!QuestListContainer || !EntryWidgetClass || !QuestComp) return;

	// 先清空上一次的显示
	QuestListContainer->ClearChildren();

	// 获取最新的任务数据
	TArray<FPanLingQuestDisplayInfo> ActiveQuests = QuestComp->GetActiveQuestsInfo();

	// 动态生成 UI 条目并添加到垂直框中
	for (const FPanLingQuestDisplayInfo& QuestInfo : ActiveQuests)
	{
		UPanLingQuestEntryWidget* Entry = CreateWidget<UPanLingQuestEntryWidget>(this, EntryWidgetClass);
		if (Entry)
		{
			Entry->SetupEntry(QuestInfo);
			QuestListContainer->AddChildToVerticalBox(Entry);
		}
	}
}
