// Fill out your copyright notice in the Description page of Project Settings.


#include "PanLingDialogueWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"


void UPanLingDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 将按钮的点击事件绑定到我们的C++函数上
	if (NextButton)
	{
		NextButton->OnClicked.AddDynamic(this, &UPanLingDialogueWidget::OnNextButtonClicked);
	}
}

void UPanLingDialogueWidget::StartDialogue(UDataTable* DialogueTable, FName StartRowName, APlayerController* PC)
{
	CurrentDataTable = DialogueTable;
	CurrentRowName = StartRowName;
	CurrentPC = PC;

	if (CurrentPC)
	{
		// 将游戏模式切换为仅UI响应，并显示鼠标指针
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		CurrentPC->SetInputMode(InputMode);
		CurrentPC->bShowMouseCursor = true;
	}

	// 第一次显示
	UpdateUI();
}

void UPanLingDialogueWidget::UpdateUI()
{
	// 如果数据表为空，或者当前行名为空(即填了 None)，说明对话结束
	if (!CurrentDataTable || CurrentRowName.IsNone())
	{
		EndDialogue();
		return;
	}

	static const FString ContextString(TEXT("Dialogue Context"));
	// 从数据表中根据 RowName 获取那一行的结构体数据
	FPanLingDialogueData* Row = CurrentDataTable->FindRow<FPanLingDialogueData>(CurrentRowName, ContextString);

	if (Row)
	{
		// 设置UI文本
		SpeakerNameText->SetText(FText::FromName(Row->SpeakerName));
		DialogueBodyText->SetText(Row->DialogueText);

		// 如果这一句包含任务，我们可以在这里通过委托(Delegate)把 QuestID 发送给玩家的任务组件 (后续补充)
		if (Row->bHasQuest)
		{
			// TODO: Fire GiveQuest Event
			UE_LOG(LogTemp, Warning, TEXT("NPC gives quest: %s"), *Row->QuestID.ToString());
		}

		// 把下一行的ID存起来，等玩家按 Next 按钮时再去读
		CurrentRowName = Row->NextRowID;
	}
	else
	{
		EndDialogue();
	}
}

void UPanLingDialogueWidget::OnNextButtonClicked()
{
	// 每次点击，再次调用 UpdateUI，因为 CurrentRowName 已经变成下一句了
	UpdateUI();
}

void UPanLingDialogueWidget::EndDialogue()
{
	// 从屏幕上移除这个UI
	RemoveFromParent();

	if (CurrentPC)
	{
		// 恢复游戏控制，隐藏鼠标
		FInputModeGameOnly InputMode;
		CurrentPC->SetInputMode(InputMode);
		CurrentPC->bShowMouseCursor = false;
	}
}
