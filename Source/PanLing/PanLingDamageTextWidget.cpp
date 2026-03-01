// Fill out your copyright notice in the Description page of Project Settings.


#include "PanLingDamageTextWidget.h"
#include "Components/TextBlock.h"

void UPanLingDamageTextWidget::SetDamageText(float DamageAmount)
{
	if (DamageText)
	{
		// 1. 保留两位小数（四舍五入）
		float Damagefloat = FMath::RoundToFloat(DamageAmount * 100.0f) / 100.0f;

		// 2. 将数字转换为 FText 并设置给 TextBlock
		DamageText->SetText(FText::AsNumber(Damagefloat));
	}
}
