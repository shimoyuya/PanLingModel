// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PanLingDamageTextWidget.generated.h"

class UTextBlock;

/**
 * 浮动伤害数字的 UI 基类
 */
UCLASS()
class PANLING_API UPanLingDamageTextWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// meta=(BindWidget) 要求蓝图中必须有一个一模一样名字的 UTextBlock
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DamageText;

public:
	// 设置伤害数字的方法
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetDamageText(float DamageAmount);
	
};
