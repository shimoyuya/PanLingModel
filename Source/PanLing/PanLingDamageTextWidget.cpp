// Fill out your copyright notice in the Description page of Project Settings.


#include "PanLingDamageTextWidget.h"
#include "Components/TextBlock.h"

void UPanLingDamageTextWidget::SetDamageText(float DamageAmount)
{
	if (DamageText)
	{
		// 1. 将浮点数转换为整数（四舍五入）
		int32 DamageInt = FMath::RoundToInt(DamageAmount);

		// 2. 将数字转换为 FText 并设置给 TextBlock
		DamageText->SetText(FText::AsNumber(DamageInt));
	}
}
