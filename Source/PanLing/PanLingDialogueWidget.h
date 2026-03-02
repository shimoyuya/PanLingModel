// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PanLingDialogueTypes.h"
#include "PanLingDialogueWidget.generated.h"

class UTextBlock;
class UButton;

/**
 对话的 UI 基类
 */
UCLASS()
class PANLING_API UPanLingDialogueWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	// UI初始化时调用，用来绑定按钮事件
	virtual void NativeConstruct() override;

	// 绑定蓝图中的名字文本
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SpeakerNameText;

	// 绑定蓝图中的对话内容文本
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DialogueBodyText;

	// 绑定蓝图中的“下一步”按钮
	UPROPERTY(meta = (BindWidget))
	UButton* NextButton;

	// 按钮点击的回调函数
	UFUNCTION()
	void OnNextButtonClicked();

public:
	// 对外接口：NPC调用这个函数来启动UI并传入数据表
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartDialogue(class UDataTable* DialogueTable, FName StartRowName, class APlayerController* PC);

private:
	// 缓存在内存里的状态
	class UDataTable* CurrentDataTable;
	FName CurrentRowName;
	class APlayerController* CurrentPC;

	// 刷新UI显示
	void UpdateUI();
	// 结束对话并销毁UI
	void EndDialogue();

};
